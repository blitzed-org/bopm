#ifndef LOG_H
#define LOG_H

extern void log_open(char *filename);
extern void log_close(void);
extern void log(char *data, ...);

#endif

