#include "PlcCraft.h"

PlcCraft::PlcCraft() {
  gas_ = new Gas;
  follower_ = new Follower;
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
    return PLC_ERROR;
  }
  if (CheckPostCondition() == PLC_TASK_EXEC_DONE) {
    return PLC_DONE;
  } else {
    return PLC_EXEC;
  }
}

PLC_TASK_EXEC_ENUM PlcCraft::CheckPostCondition() {
  switch (cmd_) {
    case FOLLOW_CUTTING_HEIGHT:
    case FOLLOW_FIRST_HEIGHT:
    case FOLLOW_SECOND_HEIGHT:
    case FOLLOW_THIRD_HEIGHT:
      return PLC_TASK_EXEC_WAITING_FOR_LHC;
    default:
      return PLC_TASK_EXEC_DONE;
      break;
  }
}

int PlcCraft::DoCmd() {
  int retval = 0;
  if (GetCmdQueueSize() != 0) {
    cmd_ = GetNextCmd();
    switch (cmd_) {
      // Gas Command
      case OPEN_CUTTING_GAS:
        retval = gas_->Open(current_layer_, CRAFT_CUTTING); 
        break;
      case OPEN_FIRST_GAS:
        retval = gas_->Open(current_layer_, CRAFT_FIRST); 
        break;
      case OPEN_SECOND_GAS:
        retval = gas_->Open(current_layer_, CRAFT_SECOND); 
        break;
      case OPEN_THIRD_GAS:
        retval = gas_->Open(current_layer_, CRAFT_THIRD); 
        break;
      case CLOSE_CUTTING_GAS:
        retval = gas_->Close(current_layer_, CRAFT_CUTTING); 
        break;
      case CLOSE_FIRST_GAS:
        retval = gas_->Close(current_layer_, CRAFT_FIRST); 
        break;
      case CLOSE_SECOND_GAS:
        retval = gas_->Close(current_layer_, CRAFT_SECOND); 
        break;
      case CLOSE_THIRD_GAS:
        retval = gas_->Close(current_layer_, CRAFT_THIRD); 
        break;
      case SET_CUTTING_PRESSURE:
        retval = gas_->SetPressure(current_layer_, CRAFT_CUTTING); 
        break;
      case SET_FIRST_PRESSURE:
        retval = gas_->SetPressure(current_layer_, CRAFT_FIRST); 
        break;
      case SET_SECOND_PRESSURE:
        retval = gas_->SetPressure(current_layer_, CRAFT_SECOND); 
        break;
      case SET_THIRD_PRESSURE:
        retval = gas_->SetPressure(current_layer_, CRAFT_THIRD); 
        break;

        // Follower Command
      case FOLLOW_CUTTING_HEIGHT:
        retval = follower_->FollowTo(current_layer_, CRAFT_CUTTING);
        break;
      case FOLLOW_FIRST_HEIGHT:
        retval = follower_->FollowTo(current_layer_, CRAFT_FIRST);
        break;
      case FOLLOW_SECOND_HEIGHT:
        retval = follower_->FollowTo(current_layer_, CRAFT_SECOND);
        break;
      case FOLLOW_THIRD_HEIGHT:
        retval = follower_->FollowTo(current_layer_, CRAFT_THIRD);
        break;
      case FIRST_PROGRESSIVE: 
        retval = follower_->IncrFollowTo(current_layer_, CRAFT_FIRST);
        break;
      case SECOND_PROGRESSIVE:
        retval = follower_->IncrFollowTo(current_layer_, CRAFT_SECOND);
        break;
      case THRED_PROGRESSIVE:
        retval = follower_->IncrFollowTo(current_layer_, CRAFT_THIRD);
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

  if (gas_->status_ == PLC_ERROR || \
      follower_->status_ == PLC_ERROR) {

    status_ = PLC_ERROR;
  } else if (gas_->status_ == PLC_DONE && \
      follower_->status_ == PLC_DONE) {
    
    status_ = PLC_DONE;
  } else {
    status_ = PLC_EXEC;
  }
}
