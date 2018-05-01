#ifndef JOBLOADER_H_
#define JOBLOADER_H_

#include "FlBase.h"

#include <queue>

#include "../devices/dev_cfg/PlcCfg.h"

class PlcCraft;

class JobLoader {
 public:
  JobLoader();
  void LoadProcesses(const PlcJobInfo &info, std::queue<PlcCmd> &cmd_queue);

 private:

  PlcCfg plc_cfg_;
  std::vector<ProcessCfg> process_cfg_;
  
  void LoadM07(int layer, std::queue<PlcCmd> &cmd_queue);
  void LoadM08(const PlcJobInfo &info, std::queue<PlcCmd> &cmd_queue);
  void AppendPlcCmdToQueue(std::vector<PlcCmd> &cmds,
      std::queue<PlcCmd> &cmd_queue);

  friend class PlcCraft;
};


#endif // JOBLOADE_H_
