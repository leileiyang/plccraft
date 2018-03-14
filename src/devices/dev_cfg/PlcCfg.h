#ifndef PLCCFG_H_
#define PLCCFG_H_

#include <string>
#include <iostream>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

// Auto command macro format: device_operate_level
// e.g. GAS_OPEN_CUTTING
//      GAS_OPEN_FIRST
//      LHC_FOLLOW_CUTTING
//      LHC_FOLLOW_FIRST
//      LHC_PROGRESSIVE_FIRST
//

#define CRAFT_LAYERS 18

#define CRAFT_LEVELS 4

#define IO_PORT_SIZE 17

enum CRAFT_LEVEL {
    CRAFT_CUTTING = 0,
    CRAFT_FIRST = 1,
    CRAFT_SECOND = 2,
    CRAFT_THIRD = 3,
};

enum IO_OP_MODE {
  PORT_NUM,
  FUNC_ID,
};

enum GasType {
  VACUO,
  AIR,
  OXYGEN,
  NITROGEN,
  HIGH_AIR,
  HIGH_OXYGEN,
  HIGH_NITROGEN,
};

enum PLC_CMD_ENUM {
  // 
  PLC_CMD_NONE = 0,
  // Gas Command
  GAS_OPEN_CUTTING = 100,
  GAS_OPEN_FIRST = 101,
  GAS_OPEN_SECOND = 102,
  GAS_OPEN_THIRD = 103,

  GAS_CLOSE_CUTTING = 104,
  GAS_CLOSE_FIRST = 105,
  GAS_CLOSE_SECOND = 106,
  GAS_CLOSE_THIRD = 107,

  GAS_PRESSURE_CUTTING = 108,
  GAS_PRESSURE_FIRST = 109,
  GAS_PRESSURE_SECOND = 110,
  GAS_PRESSURE_THIRD = 111,

  // Follower Command
  LHC_FOLLOW_CUTTING = 120,
  LHC_FOLLOW_FIRST = 121,
  LHC_FOLLOW_SECOND = 122,
  LHC_FOLLOW_THIRD = 123,

  LHC_PROGRESSIVE_FIRST = 124,
  LHC_PROGRESSIVE_SECOND = 125,
  LHC_PROGRESSIVE_THIRD = 126,

  LHC_LIFT = 127,

  // Laser Command
  LASER_ON = 140,
  LASER_OFF = 141,
  LASER_SHUTTER_ON = 142,
  LASER_SHUTTER_OFF = 143,
  LASER_POWER_CUTTING = 144,
  LASER_POWER_FIRST = 145,
  LASER_POWER_SECOND = 146,
  LASER_POWER_THIRD = 147,
  LASER_DUTYRATIO_CUTTING = 148,
  LASER_DUTYRATIO_FIRST = 149,
  LASER_DUTYRATIO_SECOND = 150,
  LASER_DUTYRATIO_THIRD = 151,
  LASER_PULSEFREQ_CUTTING = 152, 
  LASER_PULSEFREQ_FIRST = 153, 
  LASER_PULSEFREQ_SECOND = 154, 
  LASER_PULSEFREQ_THIRD = 155, 

  // Delay Command
  DELAY_CUTTING_= 180,
  DELAY_FIRST = 181,
  DELAY_SECOND = 182,
  DELAY_THIRD = 183,

  DELAY_BLOW_CUTTING = 184,
  DELAY_BLOW_FIRST = 185,
  DELAY_BLOW_SECOND = 186,
  DELAY_BLOW_THIRD = 187,

  DELAY_STAY_CUTTING = 188,
  DELAY_STAY_FIRST = 189,
  DELAY_STAY_SECOND = 190,
  DELAY_STAY_THIRD = 191,

  // Alarm function id
  GAS_LPRESSURE_ALARM = 300,
  GAS_HPRESSURE_ALARM = 301,
  GAS_AIR_ALARM = 302,
  GAS_O2_ALARM = 303,
  GAS_N2_ALARM = 304,

  // Gas function id
  GAS_AIR = 400,
  GAS_O2 = 401,
  GAS_N2 = 402,
  GAS_HIGH_AIR = 403,
  GAS_HIGH_O2 = 404,
  GAS_HIGH_N2 = 405,

};

struct PlcCmd {
  int cmd_id;
  std::string args;
};



class PlcCfg {
 public:
  std::vector<PlcCmd> cutting_;
  std::vector<PlcCmd> pierce1_;
  std::vector<PlcCmd> pierce2_;
  std::vector<PlcCmd> pierce3_;
  std::vector<PlcCmd> stripping_;
  std::vector<PlcCmd> cooling_;
  std::vector<PlcCmd> laser_off_;
  std::vector<PlcCmd> laser_off_short_;

  void Show() {
    for (std::vector<PlcCmd>::iterator it = cutting_.begin();
        it != cutting_.end(); it++) { 
    
      std::cout << it->cmd_id << ":" << it->args << std::endl;\
    }
    for (std::vector<PlcCmd>::iterator it = pierce1_.begin();
        it != pierce1_.end(); it++) { 

      std::cout << it->cmd_id << ":" << it->args << std::endl;\
    }
    for (std::vector<PlcCmd>::iterator it = pierce2_.begin();
        it != pierce2_.end(); it++) { 

      std::cout << it->cmd_id << ":" << it->args << std::endl;\
    }
    for (std::vector<PlcCmd>::iterator it = pierce3_.begin();
        it != pierce3_.end(); it++) { 

      std::cout << it->cmd_id << ":" << it->args << std::endl;\
    }
    for (std::vector<PlcCmd>::iterator it = cooling_.begin();
        it != cooling_.end(); it++) { 

      std::cout << it->cmd_id << ":" << it->args << std::endl;\
    }
    for (std::vector<PlcCmd>::iterator it = stripping_.begin();
        it != stripping_.end(); it++) { 

      std::cout << it->cmd_id << ":" << it->args << std::endl;\
    }
    for (std::vector<PlcCmd>::iterator it = laser_off_.begin();
        it != laser_off_.end(); it++) { 

      std::cout << it->cmd_id << ":" << it->args << std::endl;\
    }
    for (std::vector<PlcCmd>::iterator it = laser_off_short_.begin();
        it != laser_off_short_.end(); it++) { 

      std::cout << it->cmd_id << ":" << it->args << std::endl;\
    }
  }
};

namespace boost {
namespace serialization {

template <class Archive>
void serialize(Archive &ar, PlcCmd &cfg, const unsigned int version) {
  ar & cfg.cmd_id;
  ar & cfg.args;
}

template <class Archive>
void serialize(Archive &ar, PlcCfg &cfg, const unsigned int version) {
  ar & cfg.cutting_;
  ar & cfg.pierce1_;
  ar & cfg.pierce2_;
  ar & cfg.pierce3_;
  ar & cfg.stripping_;
  ar & cfg.cooling_;
  ar & cfg.laser_off_;
  ar & cfg.laser_off_short_;
}

}
}

#endif
