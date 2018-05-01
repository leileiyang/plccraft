#include "JobLoader.h"

#include "PlcCraft.h"

JobLoader::JobLoader(): process_cfg_(CRAFT_LAYERS, ProcessCfg()) {}

void JobLoader::LoadProcesses(const PlcJobInfo &info,
    std::queue<PlcCmd> &cmd_queue) {

  switch (info.operation) {
  case JOB_M07:
    LoadM07(info.job_layer, cmd_queue);
    break;
  case JOB_M08:
    LoadM08(info, cmd_queue);
    break;
  default:
    break;
  }
}

void JobLoader::LoadM07(int layer, std::queue<PlcCmd> &cmd_queue) {
  switch (process_cfg_[layer].craft_level) {
    case CRAFT_CUTTING:
      AppendPlcCmdToQueue(plc_cfg_.cutting_, cmd_queue);
      break;
    case CRAFT_FIRST:
      AppendPlcCmdToQueue(plc_cfg_.pierce1_, cmd_queue);
      break;
    case CRAFT_SECOND:
      AppendPlcCmdToQueue(plc_cfg_.pierce2_, cmd_queue);
      break;
    case CRAFT_THIRD:
      AppendPlcCmdToQueue(plc_cfg_.pierce3_, cmd_queue);
      break;
    case CRAFT_STRIPING:
      AppendPlcCmdToQueue(plc_cfg_.striping_, cmd_queue);
      break;
    case CRAFT_COOLING:
      AppendPlcCmdToQueue(plc_cfg_.cooling_, cmd_queue);
      break;
    default:
      break;
  }
}

void JobLoader::LoadM08(const PlcJobInfo & /*info*/,
    std::queue<PlcCmd> &cmd_queue) {

  AppendPlcCmdToQueue(plc_cfg_.laser_off_, cmd_queue);
}

void JobLoader::AppendPlcCmdToQueue(std::vector<PlcCmd> &cmds,
    std::queue<PlcCmd> &cmd_queue) {

  for (std::vector<PlcCmd>::iterator iter = cmds.begin(); iter != cmds.end();
      iter++) {

    cmd_queue.push(*iter);
  }
}

