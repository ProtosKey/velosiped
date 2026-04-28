#ifndef VLS_TIME
#define VLS_TIME

#include <time.h>
#include <unistd.h>

#define TIME_BUF_SIZE 64
#define FMT "%Y-%m-%d %H:%M:%S\n"

extern int write_time_to_vls(int fd);
time_t parse_from_fd(int fd);

#endif
