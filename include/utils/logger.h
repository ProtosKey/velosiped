#ifndef VLS_ERROR_LOGGER_H
#define VLS_ERROR_LOGGER_H

int vls_quick(const char *msg);
int vls_say(const char *msg);
int vls_report(const char *msg);
int vls_report_at(const char *ctx, const char *msg);
int vls_report_errno(int err);
int vls_report_errno_at(const char *ctx, int err);

#endif
