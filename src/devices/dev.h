#ifndef DEV_DEV_H_
#define DEV_DEV_H_

enum PLC_STATUS {
  PLC_DONE = 1,
  PLC_EXEC = 2,
  PLC_ERRROR = 3,
};

struct PortCfg {
  PortCfg(): group(0), num(0) {}
  int group;
  int num;
};

struct IoAlarmItem {
  IoAlarmItem(): alarm_bit_pos(0), normal_open(0) {}
  int alarm_bit_pos;
  int normal_open;
  PortCfg port;
};

struct GeneralAlarmMsg {
  GeneralAlarmMsg(int alarm_id, int on_off): id(alarm_id),
  on(on_off) {}

  int id;
  int on;
};

#endif
