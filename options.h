#ifndef OPTIONS_H
#define OPTIONS_H
  
/* The default name for conf, log, pid files */
#define DEFAULTNAME "bopm"

/* file extensions */
/* config */
#define CONFEXT "conf"
/* log file */
#define LOGEXT "log"
/* PID file */
#define PIDEXT "pid"

/* Defines time in which bot will timeout * if no data is received
 * (default 15 min) */
#define NODATA_TIMEOUT 900

/* If defined, BOPM will force an Unreal server to use Hybrid style
 * +c notices (PROTOCTL HCN)
 */
#undef UNREAL

/* Use poll() instead of select() */
#define USE_POLL
#define MAX_POLL 1024
                
#endif /* OPTIONS_H */
