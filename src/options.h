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

/* Max data to read from any port before closing the connection
 * (to avoid flood attempts)
 */
#define MAXREAD 4096

/* How many nodes to take off the warnq and send as notices, each time.
 * Currently the warnloop is run every second, so this equates to NOTICEs per
 * second.  If you do too many them bopm will get SendQ exceeded.
 */
#define NOTICES_PER_LOOP 5
     
#endif /* OPTIONS_H */
