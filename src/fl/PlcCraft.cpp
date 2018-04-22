#include "PlcCraft.h"

#include "timer.hh"
#include "../devices/gas/IOGas.h"

PlcCraft::PlcCraft(): current_layer_(0), status_(PLC_DONE),
    exec_state_(PLC_EXEC_DONE), execute_error_(0),
    delay_cfg_(CRAFT_LAYERS, DelayCfg()),
    follower_cfg_(CRAFT_LAYERS, FollowerCfg()),
    gas_cfg_(CRAFT_LAYERS, GasCfg()),
    delay_timeout_(0.0), delay_left_(0.0) {

  output_ = new IODevice;
  gas_ = new Gas;
  follower_ = new Follower;
}

PlcCraft::~PlcCraft() {
  delete gas_;
  delete follower_;
  delete output_;
}

bool PlcCraft::Initialize() {
  // gas device initialization
  IOGas *io_gas = new IOGas(output_);
  gas_->ConnectInterface(io_gas);

  // zmq socket initialization
  if (cfg_subscriber_.InitCfgSocket()) {
    return false;
  }
  return true;
}



bool PlcCraft::OpenJobImage(const char *file_name) {
  return job_seeker_.Open(file_name);
}

bool PlcCraft::ReOpenJobImage() {
  return job_seeker_.ReOpen();
}

void PlcCraft::CloseJobImage() {
  job_seeker_.Close();
}

void PlcCraft::LoadCraftProcesses(int motion_line) {
  execute_error_ = 0;
  PlcJobInfo job_info = job_seeker_.GetPlcJobInfo(motion_line);
  if (job_info.operation == JOB_M07) {
    current_layer_ = job_info.job_layer;
  }
  job_loader_.LoadProcesses(job_info, cmds_);
}

void PlcCraft::AddCmd(const PlcCmd &command) {
  cmds_.push(command);
}



PLC_STATUS PlcCraft::Execute() {
  switch (exec_state_) {
    case PLC_EXEC_DONE:
      IssueCmd();
      break;
    case PLC_EXEC_ERROR:
      TaskAbort();
      break;
    case PLC_EXEC_WAITING_FOR_LHC:
      if (follower_->status_ == PLC_ERROR) {
        exec_state_ = PLC_EXEC_ERROR;
      } else if (follower_->status_ == PLC_DONE) {
        exec_state_ = PLC_EXEC_DONE;
      }
      break;
    case PLC_EXEC_WAITING_FOR_GAS:
      if (gas_->status_ == PLC_ERROR) {
        exec_state_ = PLC_EXEC_ERROR;
      } else if (gas_->status_ == PLC_DONE) {
        exec_state_ = PLC_EXEC_DONE;
      }
      break;
    case PLC_EXEC_WAITING_FOR_DELAY:
      delay_left_ = delay_timeout_ - etime();
      if (etime() >= delay_timeout_) {
        exec_state_ = PLC_EXEC_DONE;
        delay_left_ = 0;
      }
      break;
    default:
      break;
  }
  Update();
  return status_;
}

void PlcCraft::TaskAbort() {
  cmd_.cmd_id = PLC_CMD_NONE;
  cmds_ = std::queue<PlcCmd>();
  exec_state_ = PLC_EXEC_DONE;
  execute_error_ = 0;
  follower_->Close();
  gas_->Close();
}

PLC_STATUS PlcCraft::IssueCmd() {
  if (DoCmd() != 0) {
    execute_error_ = 1;
    return PLC_ERROR;
  }
  exec_state_ = CheckPostCondition();
}

PLC_EXEC_ENUM PlcCraft::CheckPostCondition() {
  switch (cmd_.cmd_id) {
    case LHC_FOLLOW_CUTTING:
    case LHC_FOLLOW_FIRST:
    case LHC_FOLLOW_SECOND:
    case LHC_FOLLOW_THIRD:
      return PLC_EXEC_WAITING_FOR_LHC;
      break;
    case DELAY_STAY_CUTTING:
    case DELAY_STAY_FIRST:
    case DELAY_STAY_SECOND:
    case DELAY_STAY_THIRD:
    case DELAY_BLOW_CUTTING:
    case DELAY_BLOW_FIRST:
    case DELAY_BLOW_SECOND:
    case DELAY_BLOW_THIRD:
      return PLC_EXEC_WAITING_FOR_DELAY;
      break;
    default:
      return PLC_EXEC_DONE;
      break;
  }
}

int PlcCraft::DoCmd() {
  int retval = 0;
  if (GetCmdQueueSize() != 0) {
    cmd_ = GetNextCmd();
    switch (cmd_.cmd_id) {
      // Gas Command
      case GAS_OPEN_CUTTING:
      case GAS_OPEN_FIRST:
      case GAS_OPEN_SECOND:
      case GAS_OPEN_THIRD:
        retval = OpenGas(cmd_.cmd_id - GAS_OPEN_CUTTING);
        break;
      case GAS_CLOSE_CUTTING:
      case GAS_CLOSE_FIRST:
      case GAS_CLOSE_SECOND:
      case GAS_CLOSE_THIRD:
        retval = CloseGas(cmd_.cmd_id - GAS_CLOSE_CUTTING);
        break;
      case GAS_PRESSURE_CUTTING:
      case GAS_PRESSURE_FIRST:
      case GAS_PRESSURE_SECOND:
      case GAS_PRESSURE_THIRD:
        retval = SetPressure(cmd_.cmd_id - GAS_PRESSURE_CUTTING);
        break;

        // Follower Command
      case LHC_FOLLOW_CUTTING:
      case LHC_FOLLOW_FIRST:
      case LHC_FOLLOW_SECOND:
      case LHC_FOLLOW_THIRD:
        retval = FollowTo(cmd_.cmd_id - LHC_FOLLOW_CUTTING);
        break;
      case LHC_PROGRESSIVE_FIRST:
      case LHC_PROGRESSIVE_SECOND:
      case LHC_PROGRESSIVE_THIRD:
        retval = IncrFollowTo(cmd_.cmd_id - LHC_PROGRESSIVE_CUTTING);
        break;

        // Delay Command
      case DELAY_STAY_CUTTING:
      case DELAY_STAY_FIRST:
      case DELAY_STAY_SECOND:
      case DELAY_STAY_THIRD:
        retval = StayCommand(cmd_.cmd_id - DELAY_STAY_CUTTING);
        break;
      case DELAY_BLOW_CUTTING:
      case DELAY_BLOW_FIRST:
      case DELAY_BLOW_SECOND:
      case DELAY_BLOW_THIRD:
        retval = BlowDelayCommand(cmd_.cmd_id - DELAY_BLOW_CUTTING);
        break;
      default:
        break;
    }
    DetachLastCmd();
  }
  return retval;
}

void PlcCraft::DetachLastCmd() {
  cmds_.pop();
}

const PlcCmd PlcCraft:: GetNextCmd() {
  return cmds_.front();
}

const std::size_t PlcCraft::GetCmdQueueSize() {
  return cmds_.size();
}

int PlcCraft::OpenGas(int craft_level) {
  assert(craft_level < CRAFT_LEVELS);
  return gas_->Open(gas_cfg_[current_layer_].gas_[craft_level]);
}

int PlcCraft::CloseGas(int craft_level) {
  assert(craft_level < CRAFT_LEVELS);
  return gas_->Close(gas_cfg_[current_layer_].gas_[craft_level]);
}

int PlcCraft::SetPressure(int craft_level) {
  assert(craft_level < CRAFT_LEVELS);
  return gas_->SetPressure(gas_cfg_[current_layer_].gas_[craft_level], \
      gas_cfg_[current_layer_].pressure_[craft_level]); 

}

int PlcCraft::FollowTo(int craft_level) {
  assert(craft_level < CRAFT_LEVELS);
  if (follower_cfg_[current_layer_].no_follow_) {
    return 0;
  }
  if (follower_->FollowTo(follower_cfg_[current_layer_].height_[craft_level])) {
    return 0; 
  } else {
    return -1;
  }
}

int PlcCraft::IncrFollowTo(int craft_level) {
  assert(craft_level < CRAFT_LEVELS);
  if (follower_cfg_[current_layer_].no_follow_) {
    return 0;
  }

  if (craft_level == CRAFT_CUTTING) {
      return 0;
  }
  if (!follower_cfg_[current_layer_].incr_enable_[craft_level]) {
      return 0;
  }
  if (follower_->IncrFollowTo(\
      follower_cfg_[current_layer_].height_[craft_level-1],\
      follower_cfg_[current_layer_].incr_time_[craft_level])) {

    return 0; 
  } else {
    return -1;
  }
}

int PlcCraft::LiftTo() {
  if (follower_->LiftTo(follower_cfg_[current_layer_].lift_height_)) {
    return 0; 
  } else {
    return -1;
  }
}

int PlcCraft::BlowDelayCommand(int craft_level) {
  if (delay_cfg_[current_layer_].blow_enable_[craft_level]) {
    DelayCommand(delay_cfg_[current_layer_].laser_off_blow_time_[craft_level]);
  }
  return 0;
}

int PlcCraft::StayCommand(int craft_level) {
  return DelayCommand(delay_cfg_[current_layer_].stay_[craft_level]);
}

int PlcCraft::DelayCommand(double time) {
  delay_timeout_ = etime() + time; 
  return 0;
}

void PlcCraft::Update() {
  gas_->Update();
  follower_->Update();
  // Update Laser

  if (execute_error_ || gas_->status_ == PLC_ERROR || \
      follower_->status_ == PLC_ERROR) {

    status_ = PLC_ERROR;
  } else if (!execute_error_ && gas_->status_ == PLC_DONE && \
      follower_->status_ == PLC_DONE && \
      GetCmdQueueSize() == 0) {

    status_ = PLC_DONE;
  } else {
    status_ = PLC_EXEC;
  }
}

void PlcCraft::UpdateDeviceCfg() {
  cfg_subscriber_.UpdateGasCfg(gas_cfg_);
  cfg_subscriber_.UpdateFollowerCfg(follower_cfg_);
  cfg_subscriber_.UpdatePlcCfg(job_loader_.plc_cfg_);
  cfg_subscriber_.AckAnyReceived();
}

int PlcCraft::PullCommand(PlcCmd &cmd) {
  return cfg_subscriber_.PullCommand(cmd);
}
