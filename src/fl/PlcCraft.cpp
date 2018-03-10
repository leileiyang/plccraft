#include "PlcCraft.h"

PlcCraft::PlcCraft(): craft_layer_(0), status_(PLC_DONE),
    exec_state_(PLC_EXEC_DONE), execute_error_(0) {

  gas_ = new Gas;
  follower_ = new Follower;
}

bool PlcCraft::Initialize() {
  if (device_cfg_.InitCfgSocket()) {
    return false;
  }
  return true;
}

void PlcCraft::LoadCraft(int craft_layer) {
  execute_error_ = 0;
  craft_layer_ = craft_layer;
}

void PlcCraft::TaskAbort() {
  cmd_ = PLC_CMD_NONE;
  cmds_ = std::queue<PLC_CMD_ENUM>();
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

void PlcCraft::AddCmd(PLC_CMD_ENUM command) {
  cmds_.push(command);
}

void PlcCraft::DetachLastCmd() {
  cmds_.pop();
}

const PLC_CMD_ENUM PlcCraft:: GetNextCmd() {
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
  if (exec_state_ == PLC_EXEC_DONE) {
    return PLC_DONE;
  } else {
    return PLC_EXEC;
  }
}

PLC_EXEC_ENUM PlcCraft::CheckPostCondition() {
  switch (cmd_) {
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
    switch (cmd_) {
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
  // Update Gas
  gas_->Update();
  // Update Follower
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
  device_cfg_.UpdateGasCfg(gas_->gas_cfg_);
  device_cfg_.UpdateFollowerCfg(follower_->follower_cfg_);
  device_cfg_.AckAnyReceived();
}
