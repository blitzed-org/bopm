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

#define MAX_POLL 1024

/* Client buffer size for scan 
 * Increase as needed, but should never need to be be over 512 bytes */

#define SCANBUFFER 128
     
#endif /* OPTIONS_H */
