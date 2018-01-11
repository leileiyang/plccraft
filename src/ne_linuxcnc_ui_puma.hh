#ifndef NE_LINUXCNC_UI
#define NE_LINUXCNC_UI

#ifdef __cplusplus
extern "C" {
#endif
///////////////////////////////////////////////////////////////////////////////
//      Miscellaneous printf macros                                                
///////////////////////////////////////////////////////////////////////////////

// special fatal error signature 
#define UI_FATAL_ERROR_SIGN "NE_UI_FATAL_ERROR|"
#define fatal_err_prfmt(fmt) "\n" "%s%s:%s:%d:: " fmt, UI_FATAL_ERROR_SIGN, __FILE__, __func__, __LINE__
#define prerr(fmt, ...) fprintf(stderr, fatal_err_prfmt(fmt), ##__VA_ARGS__)

// general printf'ing 
#define prfmt(fmt) "%s:%s:%d:: " fmt, __FILE__, __func__, __LINE__
#define prinfo(fmt, ...) printf(prfmt(fmt), ##__VA_ARGS__)
#define prwarn(fmt, ...) fprintf(stderr, prfmt(fmt), ##__VA_ARGS__)
#ifdef DEBUG
#define prdbg(fmt, ...) printf(prfmt(fmt), ##__VA_ARGS__)
#else
#define prdbg(fmt, ...) do{}while(0)
#endif

#define PRINT_UI(mode, fmt, ...)  do { \
	if (mode == STAT_MODE_STDOUT) { \
		printf(fmt, ##__VA_ARGS__); \
		fflush(stdout); \
	} \
} while (0)

#define PRINT_UI_DASHED_LINE_SEPARATOR(mode) \
	PRINT_UI(mode, "------------------------------------------------------------------------------\n")

///////////////////////////////////////////////////////////////////////////////
// LinuxCNC Status Info                                                        
///////////////////////////////////////////////////////////////////////////////

/** mode of status info display (macro over enum to allow stringifying) **/
#define	STAT_MODE_STDOUT 0	// print info on stdout
#define	STAT_MODE_SHMEM 1	// status info in (posix) shm

#define	STAT_STR_LEN 80		// 80 characters wide

// status and puma-specific motion feedback info display 
#define	MDIQUEUE_STR_WIDTH 8
#define	INTERPSTATE_STR_WIDTH 11
#define	COORDSTYPE_STR_WIDTH 8
#define	VWIDTH 7
#define	STATUS_STR_FMT_PUMA \
  "%*i %*s %*s %7.2f %7.2f %7.2f %7.2f %7.2f %7.2f"

#define	PRINT_STATUS_INFO_HDR_PUMA(mode) \
	if (mode == STAT_MODE_STDOUT) { \
		putc('\r', stdout); \
	} \
	PRINT_UI(mode, "%*s %*s %*s %*s %*s %*s %*s %*s %*s\n", \
		MDIQUEUE_STR_WIDTH, "MDIQueue", INTERPSTATE_STR_WIDTH, "InterpState", \
		COORDSTYPE_STR_WIDTH, "Coords", VWIDTH, "X-Pos", \
		VWIDTH, "Y-Pos", VWIDTH, "Z-Pos", VWIDTH, "Gamma", \
		VWIDTH, "Beta", VWIDTH, "Alpha");

// general status info display 
#define PRINT_STATUS_INFO_HDR(mode) \
	if (mode == STAT_MODE_STDOUT) { \
		putc('\r', stdout); \
	} \
	PRINT_UI(mode, "%*s %*s %*s %*s %*s %*s\n", \
					12, "Cmd Queue", 12, "InterpState", \
					12, "CoordsType", 12, "X-Position", \
					12, "Y-Position", 12, "Z-Position");

// SHM file for status info 
#define UI_EMC_SHM "/ui_emc_shm"

// SHM data struct read by both linuxcnc-UI and extern prog 
	typedef struct __uiEmcStatus uiEmcStatus;

	typedef enum {
		EMC_BUSY = 0,	// set by extern prog; also default state at mach. init 
		EMC_IDLE	// set by linuxcnc_ui 
	} UI_EMC_STATE;

	struct __uiEmcStatus {
		UI_EMC_STATE state;
		char str[STAT_STR_LEN];	// updated by linuxcnc-UI; read by extern prog 
	};

#ifdef __cplusplus
}
#endif
#endif
