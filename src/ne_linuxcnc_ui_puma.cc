//////////////////////////////////////////////////////////////////////////////
// FILE : ne_linuxcnc_ui_puma.cc
//
// DESC :
//     A demo LinuxCNC UI that accepts MDI commands from stdin and sends
//     them to LinuxCNC. It is meant to serve as a template for implementing
//     a LinuxCNC interface that supports MDI command streaming from some
//     external program.
//
//     It's motion feedback info display assumes a PUMA config.
//
// See "http://nairobi-embedded.org/linuxcnc_mdi_streaming_interface.html"
//
// NOTES:
//     This code is a skeletal port of an older version and, therefore, might
//     contain some unused code or be quite delicate. Take the liberty of
//     making your own enhancements.
//
//     This rudimentary UI program is a prodigal cross-breed of:
//
//     "src/emc/usr_intf/keystick.cc",
//     "tcl/tklinuxcnc.tcl" and "src/emc/usr_intf/emcsh.cc"
//
//     ... apart from use of the "emcStatus" class (initialized and managed by
//     the "src/emc/usr_intf/shcom.cc" library) and the "IniFile" class in
//     "ini_load()", it remains straight C code.
//
// BUILD:
//      Assuming LinuxCNC-git tree in "./linuxcnc":
//
//        >> g++ ne_linuxcnc_ui_puma.cc \
//         linuxcnc/src/emc/usr_intf/shcom.cc \
//         -I linuxcnc/include/ -I linuxcnc/src/emc/usr_intf/ \
//         -L linuxcnc/lib/ -llinuxcnc -llinuxcncini -lnml \
//         -lrt -lm -Wall -Wno-comment -O2 [-Wextra] -o ne_linuxcnc_ui_puma
//
// EXEC :
//        >> LD_LIBRARY_PATH=./linuxcnc/lib ./ne_linuxcnc_ui_puma [OPTS]
//      (see "usage()" for supported options, [OPTS])
//
// Derived from work by Fred Proctor & Will Shackleford
// License: GPL Version 2
// Siro Mugabi, nairobi-embedded (c) 2014
//////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>		// lrint(3)
#include <ctype.h>
// select(3)
#include <sys/select.h>
#include <assert.h>
#include <sys/signalfd.h>
#include <signal.h>

#ifdef __cplusplus
}
#endif
#include "ne_linuxcnc_ui_puma.hh"
#include "inifile.hh"		// class IniFile
#include "emccfg.h"		// DEFAULT_TRAJ_MAX_VELOCITY
#include "shcom.hh"		// EMC_STAT *emcStatus, char error_string[]

// FlTask headers
#include <string>
#include <sstream>
#include <iterator>
#include <vector>

#include "fl/PlcCraft.h"


// a "few" globals to control emc status printout
static int status_mode = STAT_MODE_STDOUT;
static void *status_mmap = NULL;
static char status_str[STAT_STR_LEN];
static int print_status_str = 1;	// if non-zero, allow printing "status_str[]"
static int cmd_prompt_shown = 0;	// if non-zero, cmd prompt was already displayed
static int task_state = 0;
static PlcCraft plc_craft; 

// show status string header; enable printing global "status_str[]"
#define PRINT_STAT_STR_PREP(mode) do { \
	if (!print_status_str) { \
		PRINT_STATUS_INFO_HDR_PUMA(mode); \
		print_status_str = 1; \
		cmd_prompt_shown = 0; \
	} \
} while (0)

// show cmd prompt; disable printing global "status_str"
#define MDI_CMD_PROMPT() do { \
	if (!cmd_prompt_shown) { \
		ui_showCmdPrompt(); \
		cmd_prompt_shown = 1; \
		print_status_str = 0; \
	} \
} while (0)

// reset cmd prompt
#define MDI_CMD_PROMPT_RESET() do { \
	print_status_str = 0; \
	cmd_prompt_shown = 0; \
} while (0)

// LinuxCNC Interp State
//#define UI_EMC_INTERP_ERROR -1
//#define UI_EMC_INTERP_IDLE   0
//#define UI_EMC_INTERP_BUSY   1
typedef enum {
  UI_EMC_INTERP_ERROR = -1,
  UI_EMC_INTERP_IDLE = 0,
  UI_EMC_INTERP_BUSY = 1,
  UI_EMC_INTERP_PAUSED = 2
} UI_EMC_INTERP_STATE;

// number of machine axes/joints
static int emc_axis_cnt = 0;

// flag for which NML file
static int use_ini_nml = 1;	// use INI setting by default

// default INI file POSITION/COORDS settings
typedef enum {
	COORD_MACHINE = 1,
	COORD_RELATIVE
} COORD_TYPE;

static COORD_TYPE coords = COORD_MACHINE;

typedef enum {
	POS_DISPLAY_ACT = 1,
	POS_DISPLAY_CMD
} POS_DISPLAY_TYPE;

static POS_DISPLAY_TYPE posDisplay = POS_DISPLAY_ACT;

//////////////////////////////////////////////////////////////////////////////
//
// FlTask Functions 
//
//////////////////////////////////////////////////////////////////////////////

typedef enum {
  EMC_TASK_ERROR = -1,
  EMC_TASK_IDLE,
  EMC_TASK_BUSY,
  EMC_TASK_PAUSED,
  EMC_TASK_WAITING_FOR_PLC
} EMC_TASK_STATE;

std::vector<std::string> StringSplit(const char *str) {
  std::string line(str);
  std::istringstream iss(line);
  std::vector<std::string> tokens;
  std::copy(std::istream_iterator<std::string>(iss),
      std::istream_iterator<std::string>(), std::back_inserter(tokens));

  return tokens;
}

int FlTaskInit() {
  if (plc_craft.Initialize()) {
    return 0;
  }
  return -1;
}

int PlcCraftJob(int motion_line) {
  plc_craft.LoadCraftProcesses(motion_line);
}

int FlTaskCommand(const PlcCmd &cmd) {
  int ret = 0;
  if (cmd.cmd_id == TASK_OPEN_FILE) {
    PRINT_UI(status_mode, ">> Open file...\n");
    ret = sendProgramOpen(const_cast<char *>(cmd.args.c_str()));
    if (ret == 0) {
      plc_craft.OpenJobImage(const_cast<char *>(cmd.args.c_str()));
    }
  } else if (cmd.cmd_id == TASK_PAUSE) {
    PRINT_UI(status_mode, ">> Pause\n");
    ret = sendProgramPause();
    plc_craft.TaskAbort();
    task_state = EMC_TASK_PAUSED;
  } else if (cmd.cmd_id == TASK_STOP) {
    PRINT_UI(status_mode, ">> Stop\n");
    ret = sendAbort();
    plc_craft.TaskAbort();
    plc_craft.CloseJobImage();
  } else if (cmd.cmd_id == TASK_START) {
    PRINT_UI(status_mode, ">> Start\n");
    ret = sendProgramRun(0);
    if (ret == 0) {
      plc_craft.ReOpenJobImage();
    }
    task_state = EMC_TASK_BUSY;
  } else if (cmd.cmd_id == TASK_RESUME) {
    PRINT_UI(status_mode, ">> Resume\n");
    ret = sendProgramResume();
  } else if (cmd.cmd_id == TASK_SWITCH_MODE) {
    PRINT_UI(status_mode, ">> Set Mode\n");
    if (cmd.args == "auto") {
      PRINT_UI(status_mode, ">> Set Auto\n");
      ret = sendAuto();
    } else if (cmd.args == "mdi") {
      PRINT_UI(status_mode, ">> Set Mdi\n");
      ret = sendMdi();
    } else if (cmd.args == "manual") {
      PRINT_UI(status_mode, ">> Set Manual\n");
      ret = sendManual();
    }
  }
  if (ret < 0) {
    task_state = EMC_TASK_ERROR;
  }
  return ret;
}

int CommandInPaused() {
  int ret = 0;
  PlcCmd cmd;
  if (plc_craft.PullCommand(cmd)) {
    switch (cmd.cmd_id) {
      case TASK_RESUME:
      case TASK_PAUSE:
      case TASK_STOP:
        PRINT_UI(status_mode, "echo command:%d->%s\n", cmd.cmd_id, cmd.args.c_str());
        ret = FlTaskCommand(cmd);
        break;
      default:
        break;
    }
  }
  return ret;
}

int CommandInBusy() {
  int ret = 0;
  PlcCmd cmd;
  if (plc_craft.PullCommand(cmd)) {
    switch (cmd.cmd_id) {
      case TASK_PAUSE:
      case TASK_STOP:
        PRINT_UI(status_mode, "echo command:%d->%s\n", cmd.cmd_id, cmd.args.c_str());
        ret = FlTaskCommand(cmd);
        break;
      default:
        break;
    }
  }
  return ret;
}


//////////////////////////////////////////////////////////////////////////////
//
// UI Timeouts and Periodic Cycle Times
//
//////////////////////////////////////////////////////////////////////////////

// (INI) DISPLAY CYCLE TIME (secs)
#define DEFAULT_DISPLAY_CYCLE 0.05	// (20Hz)
#define MIN_DISPLAY_CYCLE 0.0166	// (60Hz)
#define MAX_DISPLAY_CYCLE 0.5	// (2Hz)
static double emc_display_cycle = DEFAULT_DISPLAY_CYCLE;

#define USECS_PER_SEC  1000000L
// NML STATUS and ERROR Channel Polling Rate (usecs)
#define NML_STAT_ERR_UPDATE_CYCLE (USECS_PER_SEC/100)	// (100Hz)
// NML STATUS and ERROR Display Control
#define NML_STAT_ERR_DISPLAY_CNTR \
	lrint(emc_display_cycle * USECS_PER_SEC) / \
		NML_STAT_ERR_UPDATE_CYCLE
// AXIS HOMING DISPLAY CYCLE (usecs)
#define HOM_AXIS_DISPLAY_CYCLE \
	lrint(emc_display_cycle * USECS_PER_SEC)
// STDIN MULTIPLEX TIMEOUT (usecs)
#define UI_STDIN_TIMEOUT NML_STAT_ERR_UPDATE_CYCLE	// (100Hz)

//////////////////////////////////////////////////////////////////////////////
//
// MISC. UTILITY FUNCTIONS
//
//////////////////////////////////////////////////////////////////////////////

// Func : upcase
// Desc : destructively converts string to its uppercase counterpart
static char *upcase(char *string)
{
	char *ptr = string;

	while (*ptr) {
		*ptr = toupper(*ptr);
		ptr++;
	}
	return string;
}

//////////////////////////////////////////////////////////////////////////////
//
//  IPC UTILITY FUNCTIONS
//
//////////////////////////////////////////////////////////////////////////////

// Func : shm_init
// Desc : creates posix shm file for status info IPC; meant for use with an
//        external prog streaming in MDI cmds.
static void *shm_init(const char *const filename, size_t filesize)
{
	int fd, ret = -1;
	void *map = NULL;

	fd = shm_open(filename, O_RDWR | O_CREAT, (mode_t) 0666);
	if (fd < 0) {
		prerr("%s\n", strerror(errno));
		goto exit;
	}

	ret = ftruncate(fd, filesize);
	if (ret < 0) {
		prerr("%s\n", strerror(errno));
		goto exit;
	}

	map = mmap(0, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (map == MAP_FAILED) {
		prerr("%s\n", strerror(errno));
		goto exit;
	}

	return map;
exit:
	close(fd);
	return NULL;
}

//////////////////////////////////////////////////////////////////////////////
//
// LinuxCNC Interface Functions: RUNTIME
//
//////////////////////////////////////////////////////////////////////////////

// Func : do_updateStatus
// Desc : peek NML channel for LinuxCNC status info
static int do_updateStatus(void)
{
	// `updateStatus()` defined in `src/emc/usr_intf/shcom.cc`
	return updateStatus();
}

// Func : do_updateError
// Desc : read NML channel for LinuxCNC error messages
static int do_updateError(void)
{
	int ret = -1;

	// global `error_string` defined in `src/emc/usr_intf/shcom.cc`
	// `NML_ERROR_LEN` defined in `include/nml_io.h`
	memset(error_string, 0, NML_ERROR_LEN);

	ret = updateError(); // defined in `src/emc/usr_intf/shcom.cc`
	if (ret < 0) {
		prerr("updateError()\n");
		goto exit;
	}
	// check whether global `error_string` was updated by `updateError()`
	if (error_string[0]) {
		prerr("%s\n", error_string);
		ret = -1;
		goto exit;
	}

	ret = 0;
exit:
	return ret;
}

// Func : get_state
// Desc : updates "emcStatus" class instance and retrieves any NML error msgs
static int get_state(void)
{
	int ret;

	// obtain a "snapshot" of LinuxCNC status
	if ((ret = do_updateStatus()) < 0)
		return ret;

	// check for LinuxCNC Error messages
	return do_updateError();
}

// Func : get_interp_state
// Desc : top-level LinuxCNC interface func that peeks/reads NML channels for
//        interpreter status and error messages. Returns:
//
//        UI_EMC_INTERP_ERROR => error condition occured
//        UI_EMC_INTERP_IDLE => interpreter in idle state
//        UI_EMC_INTERP_BUSY => interpreter busy doing something
static int get_interpState(void)
{

	if (get_state() < 0)
		return UI_EMC_INTERP_ERROR;

  switch (emcStatus->task.mode) {
    case EMC_TASK_MODE_MDI:
      switch (emcStatus->task.interpState) {
        case EMC_TASK_INTERP_READING:
        case EMC_TASK_INTERP_WAITING:
        case EMC_TASK_INTERP_PAUSED:
          return UI_EMC_INTERP_BUSY;

        case EMC_TASK_INTERP_IDLE:
          task_state = EMC_TASK_IDLE;
          return UI_EMC_INTERP_IDLE;
        default: break;
      }
      break;
    case EMC_TASK_MODE_AUTO:
      switch (emcStatus->task.interpState) {
        case EMC_TASK_INTERP_READING:
        case EMC_TASK_INTERP_WAITING:
          task_state = EMC_TASK_BUSY;
          return UI_EMC_INTERP_BUSY;

        case EMC_TASK_INTERP_PAUSED:
          return UI_EMC_INTERP_PAUSED;
        case EMC_TASK_INTERP_IDLE:
          task_state = EMC_TASK_IDLE;
          return UI_EMC_INTERP_IDLE;
        default: break;
      }
      break;
    case EMC_TASK_MODE_MANUAL:
      return UI_EMC_INTERP_IDLE;
      break;

		// if control gets here, then somethig went quite wrong;
		// fall through and return error
    default:
      break;
  }
	prerr("LinuxCNC interpreter in unkown state.\n");
	return UI_EMC_INTERP_ERROR;
}

// Func : set_runtmeStatusStr
// Desc : runtime update of global "status_str" array with current
//        LinuxCNC status info
static int set_runtmeStatusStr(void)
{
	const char *interpstate, *coordtype;
	int queue;
	double x, y, z;		// tool tip position coordinates
	double a, b, c;		// roll, pitch, yaw
	#define UNUSED __attribute__((unused))
	double u UNUSED, v UNUSED, w UNUSED;

	// number of pending moves - including current mdi cmd
	queue = emcStatus->motion.traj.queue;

	// interpreter state
	switch (emcStatus->task.interpState) {
	case EMC_TASK_INTERP_READING:
		interpstate = "READING";
		break;
	case EMC_TASK_INTERP_WAITING:
		interpstate = "WAITING";
		break;
	case EMC_TASK_INTERP_PAUSED:
		interpstate = "PAUSED";
		break;
	case EMC_TASK_INTERP_IDLE:
		interpstate = "IDLE";
		break;
	default:
		prerr("LinuxCNC interpreter in unkown state.\n");
		return -1;
	}

	// coords type
	switch (coords) {
	case COORD_MACHINE:
		coordtype = "MACHINE";
		break;
	case COORD_RELATIVE:
		coordtype = "RELATIVE";
		break;
	}

	// motion feedback info
	if (coords == COORD_MACHINE) {
		// See "src/emc/usr_intf/emcsh.cc:emc_abs_act_pos()"
		if (posDisplay == POS_DISPLAY_ACT) {
			x = emcStatus->motion.traj.actualPosition.tran.x;
			y = emcStatus->motion.traj.actualPosition.tran.y;
			z = emcStatus->motion.traj.actualPosition.tran.z;
			a = emcStatus->motion.traj.actualPosition.a;
			b = emcStatus->motion.traj.actualPosition.b;
			c = emcStatus->motion.traj.actualPosition.c;
			u = emcStatus->motion.traj.actualPosition.u;
			v = emcStatus->motion.traj.actualPosition.v;
			w = emcStatus->motion.traj.actualPosition.w;
		} else {	// POS_DISPLAY_CMD
			// See "src/emc/usr_intf/emcsh.cc:emc_abs_cmd_pos()"
			x = emcStatus->motion.traj.position.tran.x;
			y = emcStatus->motion.traj.position.tran.y;
			z = emcStatus->motion.traj.position.tran.z;
			a = emcStatus->motion.traj.position.a;
			b = emcStatus->motion.traj.position.b;
			c = emcStatus->motion.traj.position.c;
			u = emcStatus->motion.traj.position.u;
			v = emcStatus->motion.traj.position.v;
			w = emcStatus->motion.traj.position.w;
		}
	} else {		// COORD_RELATIVE
		if (posDisplay == POS_DISPLAY_ACT) {
			// See "src/emc/usr_intf/emcsh.cc:emc_rel_act_pos()"
			x = emcStatus->motion.traj.actualPosition.tran.x -
			    emcStatus->task.g5x_offset.tran.x -
			    emcStatus->task.g92_offset.tran.x -
			    emcStatus->task.toolOffset.tran.x;
			y = emcStatus->motion.traj.actualPosition.tran.y -
			    emcStatus->task.g5x_offset.tran.y -
			    emcStatus->task.g92_offset.tran.y -
			    emcStatus->task.toolOffset.tran.y;
			z = emcStatus->motion.traj.actualPosition.tran.z -
			    emcStatus->task.g5x_offset.tran.z -
			    emcStatus->task.g92_offset.tran.z -
			    emcStatus->task.toolOffset.tran.z;
			a = emcStatus->motion.traj.actualPosition.a -
			    emcStatus->task.g5x_offset.a -
			    emcStatus->task.g92_offset.a -
			    emcStatus->task.toolOffset.a;
			b = emcStatus->motion.traj.actualPosition.b -
			    emcStatus->task.g5x_offset.b -
			    emcStatus->task.g92_offset.b -
			    emcStatus->task.toolOffset.b;
			c = emcStatus->motion.traj.actualPosition.c -
			    emcStatus->task.g5x_offset.c -
			    emcStatus->task.g92_offset.c -
			    emcStatus->task.toolOffset.c;
			u = emcStatus->motion.traj.actualPosition.u -
			    emcStatus->task.g5x_offset.u -
			    emcStatus->task.g92_offset.u -
			    emcStatus->task.toolOffset.u;
			v = emcStatus->motion.traj.actualPosition.v -
			    emcStatus->task.g5x_offset.v -
			    emcStatus->task.g92_offset.v -
			    emcStatus->task.toolOffset.v;
			w = emcStatus->motion.traj.actualPosition.w -
			    emcStatus->task.g5x_offset.w -
			    emcStatus->task.g92_offset.w -
			    emcStatus->task.toolOffset.w;
		} else {	// POS_DISPLAY_CMD
			// See "src/emc/usr_intf/emcsh.cc:emc_rel_cmd_pos()"
			x = emcStatus->motion.traj.position.tran.x -
			    emcStatus->task.g5x_offset.tran.x -
			    emcStatus->task.g92_offset.tran.x -
			    emcStatus->task.toolOffset.tran.x;
			y = emcStatus->motion.traj.position.tran.y -
			    emcStatus->task.g5x_offset.tran.y -
			    emcStatus->task.g92_offset.tran.y -
			    emcStatus->task.toolOffset.tran.y;
			z = emcStatus->motion.traj.position.tran.z -
			    emcStatus->task.g5x_offset.tran.z -
			    emcStatus->task.g92_offset.tran.z -
			    emcStatus->task.toolOffset.tran.z;
			a = emcStatus->motion.traj.position.a -
			    emcStatus->task.g5x_offset.a -
			    emcStatus->task.g92_offset.a -
			    emcStatus->task.toolOffset.a;
			b = emcStatus->motion.traj.position.b -
			    emcStatus->task.g5x_offset.b -
			    emcStatus->task.g92_offset.b -
			    emcStatus->task.toolOffset.b;
			c = emcStatus->motion.traj.position.c -
			    emcStatus->task.g5x_offset.c -
			    emcStatus->task.g92_offset.c -
			    emcStatus->task.toolOffset.c;
			u = emcStatus->motion.traj.position.u -
			    emcStatus->task.g5x_offset.u -
			    emcStatus->task.g92_offset.u -
			    emcStatus->task.toolOffset.u;
			v = emcStatus->motion.traj.position.v -
			    emcStatus->task.g5x_offset.v -
			    emcStatus->task.g92_offset.v -
			    emcStatus->task.toolOffset.v;
			w = emcStatus->motion.traj.position.w -
			    emcStatus->task.g5x_offset.w -
			    emcStatus->task.g92_offset.w -
			    emcStatus->task.toolOffset.w;
		}
	}

#if 0
	// For joint-space coords, instead of Base Frame coords --
	// and according to "src/emc/usr_intf/axis/extensions/emcmodule.cc":
	//       "axis[i].input" means joint actual
	//       "axis[i].output" means joint position
	//
	x = emcStatus->motion.axis[0].input;	// or ".output"
	... ... ... ... ... ...
	c = emcStatus->motion.axis[5].input;	// or ".output"
#endif

	// update local status buffer
	memset(status_str, 0, STAT_STR_LEN);
	snprintf(status_str, STAT_STR_LEN, STATUS_STR_FMT_PUMA,
		 MDIQUEUE_STR_WIDTH, queue, INTERPSTATE_STR_WIDTH, interpstate,
		 COORDSTYPE_STR_WIDTH, coordtype, x, y, z, a, b, c);

	return 0;
}

// Func : set_homAxisStatusStr
// Desc : update global "status_str" array with axis/joint
//        position feedback during homing
static void set_homAxisStatusStr(int axis)
{
	if (emcStatus->motion.axis[axis].homing) {
		memset(status_str, 0, STAT_STR_LEN);
		snprintf(status_str, STAT_STR_LEN,
			 "AXIS %i:: inpos: %12.4f, outpos: %12.4f, homing...",
			 axis,
			 emcStatus->motion.axis[axis].input,
			 emcStatus->motion.axis[axis].output);
	}

	if (emcStatus->motion.axis[axis].homed) {
		memset(status_str, 0, STAT_STR_LEN);
		snprintf(status_str, STAT_STR_LEN,
			 "AXIS %i:: inpos: %12.4f, outpos: %12.4f, [HOMED]     ",
			 axis,
			 emcStatus->motion.axis[axis].input,
			 emcStatus->motion.axis[axis].output);
	}
}

// Func : is_homed
// Desc : checks if axis/joint has reached home position;
//        returns "0" if axis/joint still homing, "1" if homed.
static int is_homed(int axis)
{
	return emcStatus->motion.axis[axis].homed;
}

//////////////////////////////////////////////////////////////////////////////
//
// LinuxCNC Interface Functions: INITIALIZATION
//
//////////////////////////////////////////////////////////////////////////////

// Func : ini_load
// Desc : parse INI file
// NOTES: doing this here, rather than `src/emc/usr_intf/shcom.cc:iniLoad()`,
//        to allow parsing what is "desirable".
static int ini_load(const char *filename)
{
	IniFile inifile;
	const char *inistring;
	char version[LINELEN] = "";
	char displayString[LINELEN] = "";
	int ret = -1;

	// open it
	if (!inifile.Open(filename))
		goto exit;

	// check for nml file
	if (use_ini_nml) {
		if (NULL != (inistring = inifile.Find("NML_FILE", "EMC"))) {
			assert(strlen(inistring) <= LINELEN - 1);
			// set global "emc_nmlfile"
			strncpy(emc_nmlfile, inistring, LINELEN - 1);
		} else {
			// using default in `include/config.h`
		}
	}

	if ((inistring = inifile.Find("MACHINE", "EMC"))) {
		PRINT_UI(status_mode, "MACHINE: %s, ", inistring);
		if ((inistring = inifile.Find("VERSION", "EMC"))) {
			if (1 == (sscanf(inistring, "$Revision: %s", version)))
				PRINT_UI(status_mode, "Version %s", version);
		}
	}
	PRINT_UI(status_mode, "\n");

	PRINT_UI(status_mode, "POSITION_OFFSET: ");
	if ((inistring = inifile.Find("POSITION_OFFSET", "DISPLAY"))) {
		if (1 == sscanf(inistring, "%s", displayString)) {
			upcase(displayString);
			if (!strcmp(displayString, "MACHINE")) {
				coords = COORD_MACHINE;
				PRINT_UI(status_mode, "MACHINE COORDS\n");
			} else if (!strcmp(displayString, "RELATIVE")) {
				coords = COORD_RELATIVE;
				PRINT_UI(status_mode, "RELATIVE COORDS\n");
			} else {
				PRINT_UI(status_mode,
					 "WARNING! Assumming MACHINE (Absolute)\n");
			}
		} else {
			PRINT_UI(status_mode,
				 "WARNING! Assumming MACHINE (Absolute)\n");
		}
	} else {
		PRINT_UI(status_mode,
			 "WARNING! Assumming MACHINE (Absolute)\n");
	}

	PRINT_UI(status_mode, "POSITION_FEEDBACK: ");
	if ((inistring = inifile.Find("POSITION_FEEDBACK", "DISPLAY"))) {
		if (1 == sscanf(inistring, "%s", displayString)) {
			upcase(displayString);
			if (!strcmp(displayString, "ACTUAL")) {
				posDisplay = POS_DISPLAY_ACT;
				PRINT_UI(status_mode, "ACTUAL\n");
			} else if (!strcmp(displayString, "COMMANDED")) {
				posDisplay = POS_DISPLAY_CMD;
				PRINT_UI(status_mode, "COMMANDED\n");
			} else {
				PRINT_UI(status_mode,
					 "WARNING! Assumming ACTUAL\n");
			}
		} else {
			PRINT_UI(status_mode, "WARNING! Assumming ACTUAL\n");
		}
	} else {
		PRINT_UI(status_mode, "WARNING! Assumming ACTUAL\n");
	}

	PRINT_UI(status_mode, "Conversion units: Linear (");
	if (NULL != (inistring = inifile.Find("LINEAR_UNITS", "DISPLAY"))) {
		if (!strcmp(inistring, "AUTO")) {
			linearUnitConversion = LINEAR_UNITS_AUTO;
			PRINT_UI(status_mode, "Auto");
		} else if (!strcmp(inistring, "INCH")) {
			linearUnitConversion = LINEAR_UNITS_INCH;
			PRINT_UI(status_mode, "Inch");
		} else if (!strcmp(inistring, "MM")) {
			linearUnitConversion = LINEAR_UNITS_MM;
			PRINT_UI(status_mode, "Millimeters");
		} else if (!strcmp(inistring, "CM")) {
			linearUnitConversion = LINEAR_UNITS_CM;
			PRINT_UI(status_mode, "Centimeters");
		}
	} else {
		PRINT_UI(status_mode, "None");
	}
	PRINT_UI(status_mode, ") ");

	PRINT_UI(status_mode, "Angular (");
	if (NULL != (inistring = inifile.Find("ANGULAR_UNITS", "DISPLAY"))) {
		if (!strcmp(inistring, "AUTO")) {
			angularUnitConversion = ANGULAR_UNITS_AUTO;
		} else if (!strcmp(inistring, "DEG")) {
			angularUnitConversion = ANGULAR_UNITS_DEG;
			PRINT_UI(status_mode, "Degrees");
		} else if (!strcmp(inistring, "RAD")) {
			angularUnitConversion = ANGULAR_UNITS_RAD;
			PRINT_UI(status_mode, "Radians");
		} else if (!strcmp(inistring, "GRAD")) {
			angularUnitConversion = ANGULAR_UNITS_GRAD;
			PRINT_UI(status_mode, "Gradians");
		}
	} else {
		PRINT_UI(status_mode, "None");
	}
	PRINT_UI(status_mode, ")");
	PRINT_UI(status_mode, "\n");	// convesion units

	// TRAJ MAX VEL
	if (NULL != (inistring = inifile.Find("MAX_VELOCITY", "TRAJ"))) {
		if (1 != sscanf(inistring, "%lf", &traj_max_velocity)) {
			traj_max_velocity = DEFAULT_TRAJ_MAX_VELOCITY;
		}
	} else {
		traj_max_velocity = DEFAULT_TRAJ_MAX_VELOCITY;
	}
	PRINT_UI(status_mode, "Traj. Max. Vel. : %lf, ", traj_max_velocity);

	// establish number of axes/joints
	if (NULL != (inistring = inifile.Find("AXES", "TRAJ"))) {
		emc_axis_cnt = atoi(inistring);
#ifdef STRICT_PUMA_CONFIG
		if (emc_axis_cnt != 6) {
			prerr("IS THIS REALLY A PUMA INI CONFIG?\n");
			ret = -1;
			goto exit;
		}
#endif
		PRINT_UI(status_mode, "AXIS/JOINT CNT: %d\n", emc_axis_cnt);
	} else {
		prerr("Must specify number of axes in INI.\n");
		ret = -1;
		goto exit;
	}

	// get display cycle time
	if (NULL != (inistring = inifile.Find("CYCLE_TIME", "DISPLAY"))) {
		if (1 != sscanf(inistring, "%lf", &emc_display_cycle)) {
			emc_display_cycle = DEFAULT_DISPLAY_CYCLE;
		}
	} else {
		emc_display_cycle = DEFAULT_DISPLAY_CYCLE;
	}

	emc_display_cycle =
	    emc_display_cycle < MIN_DISPLAY_CYCLE ? MIN_DISPLAY_CYCLE :
	    (emc_display_cycle > MAX_DISPLAY_CYCLE ? MAX_DISPLAY_CYCLE :
	     emc_display_cycle);

	PRINT_UI(status_mode, "UI Display Cycle Time (secs): %lf\n",
		 emc_display_cycle);

	// close it
	inifile.Close();

	ret = 0;
exit:
	return ret;
}

// Func : do_tryNml
// Desc : sets up all NML communication channels: command, status and error
// NOTES: tryNml() defined in `src/emc/usr_intf/shcom.cc`.
//        the "retry_*" values were copied from `src/emc/usr_intf/keystick.cc`
static int do_tryNml(void)
{
	double retry_time = 10.0;	// secs to wait for subsystems to come up
	double retry_interval = 1.0;	// secs btn wait tries for a subsystem
	return tryNml(retry_time, retry_interval);
}

//////////////////////////////////////////////////////////////////////////////
//
// User Interface Funtions
//
//////////////////////////////////////////////////////////////////////////////

// Func : ui_showCmdPrompt
// Desc : In cmdline stdin mode, resets UI to prepare for MDI command prompt
//        Otherwise, resets shm flag to indicate LinuxCNC interp idle state;
//       for use by an external program streaming in MDI commands.
static void ui_showCmdPrompt(void)
{
	switch (status_mode) {

	case STAT_MODE_SHMEM:
		// flag extern prog to send MDI cmd
		assert(status_mmap);
		((uiEmcStatus *) status_mmap)->state = EMC_IDLE;
		break;

	case STAT_MODE_STDOUT:
		PRINT_UI(status_mode, "\n\nEnter MDI Command: ");
		break;

	default:		// this should never happen
		prerr("Illegal UI status_mode\n");
		abort();
	}
}

// Func : ui_printStatusStr
// Desc : write/copy LinuxCNC status info snapshot to stdout/shm
static void ui_printStatusStr(void)
{
	char *str;
	switch (status_mode) {
	case STAT_MODE_STDOUT:
		if (print_status_str) {
			putc('\r', stdout);
			fputs(status_str, stdout);
			fflush(stdout);
		}
		break;

	case STAT_MODE_SHMEM:
		assert(status_mmap
		       && (str = ((uiEmcStatus *) status_mmap)->str));
		memset(str, 0, STAT_STR_LEN);
		status_str[STAT_STR_LEN - 1] = '\0';	// precaution and ...
		strcpy(str, status_str);	// ... cheaper than strncpy(3)
		break;

	default:		// this should never happen
		prerr("Illegal UI status_mode\n");
		abort();
	}			// switch(status_mode)
}

//////////////////////////////////////////////////////////////////////////////
//
// INITIALIZATION
//
//////////////////////////////////////////////////////////////////////////////

// Func : usage
// Desc : print cmdline usage
static void usage(char *prog)
{
	PRINT_UI(status_mode, "Usage: %s [OPTIONS]\n", prog);
	PRINT_UI(status_mode, "Options:\n" "\n"
		 "-h              help\n"
		 "-ini NAME       specify path-qualified INI filename\n"
		 "-nml NAME       specify	path-qualified NML filename\n"
		 "-statmode VAL   LinuxCNC status display mode\n" "\n"
		 "Status Mode values:\n"
		 "                 0 (default, print to stdout)\n"
		 "                 1 (copy to POSIX SHM \"%s\" file)\n",
		 UI_EMC_SHM);
	exit(EXIT_FAILURE);
}

// Func : main
// Desc : LinuxCNC initialization, polling error/status info, event-based i/o
//        for operator MDI command input, cleanup
int main(int argc, char **argv)
{
	int i, ret = -1, opt;
	/* signals */
	sigset_t mask;
	int sfd;
	struct signalfd_siginfo fdsi;

  PlcCmd cmd;
	// "getopt(3)" double colon to support multichar "-yyy" opts
	// to comply with "scripts/linuxcnc" hardcoded "-ini" option
	// passing.
	while ((opt = getopt(argc, argv, "s::i::n::h")) != -1) {
		switch (opt) {
		case 'i':
			if (!optarg || strncmp(optarg, "ni", strlen("ni"))
			    || !argv[optind]) {
				prerr("Bad \"-ini\" commandline switch\n");
				usage(argv[0]);
			}
			// copy to LinuxCNC global emc_inifile array
			memset(emc_inifile, 0, LINELEN);
			strncpy(emc_inifile, argv[optind], LINELEN - 1);
			//  a simple check if file exists
			if (open(emc_inifile, O_RDONLY) == -1) {
				prerr(" \"%s\" ; %s\n", emc_inifile,
				      strerror(errno));
				exit(EXIT_FAILURE);
			}
			break;

		case 'n':
			if (!optarg || strncmp(optarg, "ml", strlen("ml"))
			    || !argv[optind]) {
				prerr("Bad \"-nml\" commandline switch\n");
				usage(argv[0]);
			}
			// copy to LinuxCNC global emc_nmlfile array
			memset(emc_nmlfile, 0, LINELEN);
			strncpy(emc_nmlfile, argv[optind], LINELEN - 1);
			//  a simple check if file exists
			if (open(emc_nmlfile, O_RDONLY) == -1) {
				prerr(" \"%s\" ; %s\n", emc_nmlfile,
				      strerror(errno));
				exit(EXIT_FAILURE);
			}
			use_ini_nml = 0;
			break;

		case 's':
			if (!optarg
			    || strncmp(optarg, "tatmode", strlen("tatmode"))
			    || !argv[optind]) {
				prerr("Bad \"-statmode\" commandline switch\n");
				usage(argv[0]);
			}
			switch (atoi(argv[optind])) {
			case STAT_MODE_SHMEM:
				status_mode = STAT_MODE_SHMEM;	// copy to shm
				status_mmap =
				    shm_init(UI_EMC_SHM, sizeof(uiEmcStatus));
				if (!status_mmap)
					exit(EXIT_FAILURE);

				// init uiEmcStatus buffer state
				((uiEmcStatus *) status_mmap)->state = EMC_BUSY;
				memset(((uiEmcStatus *) status_mmap)->str, 0,
				       STAT_STR_LEN);
				break;
			case STAT_MODE_STDOUT:
			default:
				status_mode = STAT_MODE_STDOUT;	/* print to stdout */
				break;
			}
			break;

		case 'h':
		default:
			usage(argv[0]);
			break;
		}
	}

	////////////////////////////////////////////////////////////
	//
	// MACHINE INIT
	//
	////////////////////////////////////////////////////////////

	// parse INI file
	PRINT_UI_DASHED_LINE_SEPARATOR(status_mode);
	PRINT_UI(status_mode, ">> Parsing INI file \"%s\"...\n", emc_inifile);
	if ((ret = ini_load(emc_inifile)) < 0) {
		prerr("ini_load().\n");
		exit(EXIT_FAILURE);
	}
	PRINT_UI(status_mode, ">> Done parsing INI file.\n");

	// setup NML channels
	PRINT_UI_DASHED_LINE_SEPARATOR(status_mode);
	PRINT_UI(status_mode, ">> Using NML file \"%s\"\n", emc_nmlfile);
	PRINT_UI(status_mode, ">> Setting up NML channels...\n");
	if ((ret = do_tryNml()) < 0) {
		prerr("\"tryNml()\"; failed to set up NML channels.\n");
		exit(EXIT_FAILURE);
	}
	PRINT_UI(status_mode, ">> Done setting up NML channels.\n");

	// reset estop
	if ((ret = sendEstopReset()) < 0) {
		prerr("sendEstopReset()\n");
		exit(EXIT_FAILURE);
	}
	// switch machine on
	if ((ret = sendMachineOn()) < 0) {
		prerr("sendMachineOn()\n");
		goto exit;
	}
	// go manual mode
	if ((ret = sendManual()) < 0) {
		prerr("sendManual()\n");
		goto exit;
	}
	// home all machine axes/joints
	if ((emc_axis_cnt == 0) || (emc_axis_cnt > EMC_AXIS_MAX)) {
		prerr("Bad number of axes/joints; "
		      "Check \"[TRAJ]\" settings in INI file\n");
		goto exit;
	}

	PRINT_UI_DASHED_LINE_SEPARATOR(status_mode);
	PRINT_UI(status_mode, ">> Manual Mode: Homing axes/joints...\n");

	if (status_mode == STAT_MODE_STDOUT)
		print_status_str = 1;	// enable printing status update

  if ((ret = sendHome(-1)) < 0) {
    prerr("sendHome failed!\n");
    goto exit;
  }

	for (i = 0; i < emc_axis_cnt; i++) {
		// send axis home cmd
		//if ((ret = sendHome(i)) < 0) {
	  //	prerr("sendHome(%i)\n", i);
	  //	goto exit;
	  //}
		// poll-n-print position feedback
		while (1) {
			usleep(HOM_AXIS_DISPLAY_CYCLE);
			if ((ret = get_state()) < 0) {
				goto exit;
			}
			set_homAxisStatusStr(i);
			ui_printStatusStr();
			if (is_homed(i))
				break;
		}
		PRINT_UI(status_mode, "\n");
	}

	PRINT_UI(status_mode, ">> Done homing axes/joints.\n");

	// go mdi mode
	PRINT_UI_DASHED_LINE_SEPARATOR(status_mode);
	if ((ret = sendMdi()) < 0) {
		prerr("sendMdi()\n");
		goto exit;
	}
	PRINT_UI(status_mode, ">> MDI mode: Ready.\n");
	PRINT_UI_DASHED_LINE_SEPARATOR(status_mode);
	PRINT_STATUS_INFO_HDR_PUMA(status_mode);

	// prepare for "TERM" signal handling (also see "signal(7)")
	sigemptyset(&mask);
	sigaddset(&mask, SIGHUP);	/* e.g. PTM was closed */
	sigaddset(&mask, SIGINT);	/* infamous "CTRL+C" */
	sigaddset(&mask, SIGKILL);	/* e.g. "kill -9 $PID" */
	sigaddset(&mask, SIGPIPE);	/* broken pipe */
	sigaddset(&mask, SIGTERM);	/* e.g. "kill -15 $PID" */

	if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
		prerr("%s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if ((sfd = signalfd(-1, &mask, 0)) == -1) {
		prerr("%s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

  // FlTask INIT
  if (FlTaskInit() != 0) {
		prerr("FlTask Initialize Failed!\n");
		exit(EXIT_FAILURE);
  }

	// main loop : continuously poll LinuxCNC state (status and error)
	//             while waiting for operator MDI cmd

	for (;;) {

		char cmdbuf[LINELEN];
		fd_set rdfs;
		struct timeval to;
		static long pcntr = NML_STAT_ERR_DISPLAY_CNTR;

		ret = get_interpState();	// "peek" NML channels for status & error
		switch (ret) {

		case UI_EMC_INTERP_ERROR:
#ifdef STRICT_ERROR_CONFIG
			goto exit;
#else
			continue;
#endif

		case UI_EMC_INTERP_BUSY:	// keep polling NML channels
      CommandInBusy();
			if (status_mode == STAT_MODE_STDOUT)
				PRINT_STAT_STR_PREP(status_mode);

			usleep(NML_STAT_ERR_UPDATE_CYCLE);

			if ((pcntr--) <= 0) {
				set_runtmeStatusStr();
				ui_printStatusStr();	// print LinuxCNC status snapshot
				pcntr = NML_STAT_ERR_DISPLAY_CNTR;
			}
			continue;
    case UI_EMC_INTERP_PAUSED: // interp paused in auto mode
      CommandInPaused();
      if (task_state == EMC_TASK_PAUSED) {
        // paused by external prog
        ;
      } else { // paused by internal M0
        if (task_state == EMC_TASK_BUSY) {
          PRINT_UI(status_mode, ">> PLC Craft...\n");
          PlcCraftJob(emcStatus->task.motionLine);
          task_state = EMC_TASK_WAITING_FOR_PLC;
        } else if (task_state == EMC_TASK_WAITING_FOR_PLC) { 
          ret = plc_craft.Execute();
          if (ret == PLC_DONE) {
            cmd.cmd_id = TASK_RESUME;
            cmd.args = "";
            FlTaskCommand(cmd);
          } else if (ret == PLC_ERROR) {
            prerr("Plc Craft Error!\n");
            goto exit;
          }
        }
        continue;
      }
		case UI_EMC_INTERP_IDLE:	// check for new MDI command from operator
			// first print current LinuxCNC status snapshot
			set_runtmeStatusStr();
			ui_printStatusStr();
			// show/enable command prompt
			if (status_mode == STAT_MODE_STDOUT)
				MDI_CMD_PROMPT();

			// "select(3)"ing against STDIN_FILENO and "signalfd (sfd)"
			FD_ZERO(&rdfs);
			FD_SET(STDIN_FILENO, &rdfs);
			FD_SET(sfd, &rdfs);

			to.tv_sec = 0;
			to.tv_usec = UI_STDIN_TIMEOUT;
			ret = select(sfd + 1, &rdfs, NULL, NULL, &to);
			if (ret == -1) {
				prerr("select(3); %s\n", strerror(errno));
				goto exit;
			}

			// handle any TERM signals here
			#define SIGFDINFO_SZ sizeof(struct signalfd_siginfo)
			if (FD_ISSET(sfd, &rdfs)) {
				if ((read(sfd, &fdsi, SIGFDINFO_SZ)) !=
				    SIGFDINFO_SZ) {
					prerr("%s\n", strerror(errno));
					ret = -1;
					goto exit;
				}
				puts("");
				prinfo("Received signal %d. Quiting.\n",
				       fdsi.ssi_signo);
				puts("");
				ret = 0;
				goto exit;
			}

			// handle new task command from operator
			if (plc_craft.PullCommand(cmd)) {
        PRINT_UI(status_mode, "echo command:%d->%s\n", cmd.cmd_id, cmd.args.c_str());
        ret = FlTaskCommand(cmd);
				if (ret < 0) {
					prerr("Execute Command Failed!\n");
					goto exit;
				}

prompt_again:
				if (status_mode == STAT_MODE_STDOUT)
					MDI_CMD_PROMPT_RESET();

			} else { // FD_ISSET(STDIN_FILENO, &rdfs)
        plc_craft.UpdateDeviceCfg();
      }

			continue;	// UI_EMC_INTERP_IDLE

		default:	// this should never happen
			prerr("Unkown UI_INTERP_STATE\n");
			goto exit;
		}

	}			// while(!done)

	ret = 0;
exit:

	if ((ret = sendMachineOff()) < 0)
		prerr("sendMachineOff\n");

	if ((ret = sendEstop()) < 0)
		prerr("sendEstop\n");

	if (status_mmap)
		shm_unlink(UI_EMC_SHM);

	if (ret < 0)
		exit(EXIT_FAILURE);
	exit(EXIT_SUCCESS);
}
