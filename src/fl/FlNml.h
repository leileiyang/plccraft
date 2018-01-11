#ifndef FLNML_H_
#define FLNML_H_

#include "Fl.h"
#include <linuxcnc/cmd_msg.h>

class FL_PLC_STAT_MSG: public RCS_STAT_MSG {
 public:
  FL_PLC_STAT_MSG(NMLTYPE t, size_t s): RCS_STAT_MSG(t, s) {};

  void update(CMS *cms);
};



#endif
