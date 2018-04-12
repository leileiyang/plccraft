#include "PlcCraft.h"
#include "../devices/gas/IOGas.h"

PlcCraft::PlcCraft(): craft_layer_(0), status_(PLC_DONE),
    exec_state_(PLC_EXEC_DONE), execute_error_(0),
    process_cfg_(CRAFT_LAYERS, ProcessCfg()) {

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
  return job_image_.Open(file_name);
}

void PlcCraft::CloseJobImage() {
  job_image_.Close();
}

void PlcCraft::AppendPlcCmdToQueue(std::vector<PlcCmd> &cmds) {
  for (std::vector<PlcCmd>::iterator iter = cmds.begin(); iter != cmds.end();
      iter++) {

    AddCmd(*iter);
  }
}

void PlcCraft::LoadM07() {
  if (craft_layer_ == STRIPING_LAYER_INDEX) {
    AppendPlcCmdToQueue(plc_cfg_.striping_);
  } else if (craft_layer_ == COOLING_LAYER_INDEX) {
    AppendPlcCmdToQueue(plc_cfg_.cooling_);
  } else {
    switch (process_cfg_[craft_layer_].craft_level) {
      case CRAFT_CUTTING:
        AppendPlcCmdToQueue(plc_cfg_.cutting_);
        break;
      case CRAFT_FIRST:
        AppendPlcCmdToQueue(plc_cfg_.pierce1_);
        break;
      case CRAFT_SECOND:
        AppendPlcCmdToQueue(plc_cfg_.pierce1_);
        break;
      case CRAFT_THIRD:
        AppendPlcCmdToQueue(plc_cfg_.pierce1_);
        break;
      default:
        break;
    }
  }
}

void PlcCraft::LoadM08() {
  AppendPlcCmdToQueue(plc_cfg_.laser_off_);
}

void PlcCraft::LoadProcesses(int operation) {
  switch (operation) {
  case JOB_M07:
    LoadM07();
    break;
  case JOB_M08:
    LoadM08();
    break;
  default:
    break;
  }
}

void PlcCraft::LoadCraftProcesses(int motion_line) {
  execute_error_ = 0;
  PlcJobInfo job_info = job_image_.GetPlcJobInfo(motion_line);
  if (job_info.operation == JOB_M07) {
    craft_layer_ = job_info.job_layer;
  }
  LoadProcesses(job_info.operation);
}

void PlcCraft::TaskAbort() {
  cmd_.cmd_id = PLC_CMD_NONE;
  cmds_ = std::queue<PlcCmd>();
  exec_state_ = PLC_EXEC_DONE;
  execute_error_ = 0;
  follower_->Close();
  gas_->Close();
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
    default:
      break;
  }
  Update();
  return status_;
}

void PlcCraft::AddCmd(const PlcCmd &command) {
  cmds_.push(command);
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
        retval = gas_->Open(craft_layer_, CRAFT_CUTTING);
        break;
      case GAS_OPEN_FIRST:
        retval = gas_->Open(craft_layer_, CRAFT_FIRST);
        break;
      case GAS_OPEN_SECOND:
        retval = gas_->Open(craft_layer_, CRAFT_SECOND);
        break;
      case GAS_OPEN_THIRD:
        retval = gas_->Open(craft_layer_, CRAFT_THIRD);
        break;
      case GAS_CLOSE_CUTTING:
        retval = gas_->Close(craft_layer_, CRAFT_CUTTING);
        break;
      case GAS_CLOSE_FIRST:
        retval = gas_->Close(craft_layer_, CRAFT_FIRST);
        break;
      case GAS_CLOSE_SECOND:
        retval = gas_->Close(craft_layer_, CRAFT_SECOND);
        break;
      case GAS_CLOSE_THIRD:
        retval = gas_->Close(craft_layer_, CRAFT_THIRD);
        break;
      case GAS_PRESSURE_CUTTING:
        retval = gas_->SetPressure(craft_layer_, CRAFT_CUTTING);
        break;
      case GAS_PRESSURE_FIRST:
        retval = gas_->SetPressure(craft_layer_, CRAFT_FIRST);
        break;
      case GAS_PRESSURE_SECOND:
        retval = gas_->SetPressure(craft_layer_, CRAFT_SECOND);
        break;
      case GAS_PRESSURE_THIRD:
        retval = gas_->SetPressure(craft_layer_, CRAFT_THIRD);
        break;

        // Follower Command
      case LHC_FOLLOW_CUTTING:
        retval = follower_->FollowTo(craft_layer_, CRAFT_CUTTING);
        break;
      case LHC_FOLLOW_FIRST:
        retval = follower_->FollowTo(craft_layer_, CRAFT_FIRST);
        break;
      case LHC_FOLLOW_SECOND:
        retval = follower_->FollowTo(craft_layer_, CRAFT_SECOND);
        break;
      case LHC_FOLLOW_THIRD:
        retval = follower_->FollowTo(craft_layer_, CRAFT_THIRD);
        break;
      case LHC_PROGRESSIVE_FIRST:
        retval = follower_->IncrFollowTo(craft_layer_, CRAFT_FIRST);
        break;
      case LHC_PROGRESSIVE_SECOND:
        retval = follower_->IncrFollowTo(craft_layer_, CRAFT_SECOND);
        break;
      case LHC_PROGRESSIVE_THIRD:
        retval = follower_->IncrFollowTo(craft_layer_, CRAFT_THIRD);
        break;

        // Delay Command
      case DELAY_STAY_CUTTING:
      case DELAY_STAY_FIRST:
      case DELAY_STAY_SECOND:
      case DELAY_STAY_THIRD:
        break;
      default:
        break;
    }
    DetachLastCmd();
  }
  return retval;
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
  cfg_subscriber_.UpdateGasCfg(gas_->gas_cfg_);
  cfg_subscriber_.UpdateFollowerCfg(follower_->follower_cfg_);
  cfg_subscriber_.UpdatePlcCfg(plc_cfg_);
  cfg_subscriber_.AckAnyReceived();
}


int PlcCraft::PullCommand(PlcCmd &cmd) {
  return cfg_subscriber_.PullCommand(cmd);
}
