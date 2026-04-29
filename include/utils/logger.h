#ifndef VLS_ERROR_LOGGER_H
#define VLS_ERROR_LOGGER_H

#define CLR_RESET "\033[0m"
#define CLR_BOLD "\033[1m"
#define CLR_GREEN "\033[32m"
#define CLR_RED "\033[0;31m"
#define CLR_CYAN "\033[36m"

int vls_say(const char *);
int vls_say_green(const char *);
int vls_raw(const char *);
int vls_raw_green(const char *);

int vls_report(const char *msg);
int vls_report_at(const char *ctx, const char *msg);
int vls_report_errno(int err);
int vls_report_errno_at(const char *ctx, int err);

#endif
