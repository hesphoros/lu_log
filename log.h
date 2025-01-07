#ifndef __LU_LOG_INCLUDE_H__
#define __LU_LOG_INCLUDE_H__

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>

#define LU_LOG_VERSIO "0.1.0"

typedef struct lu_log_event_s {
	va_list ap;
	const char* fmt;
	const char* file;
	struct tm* time_info;
	void* data;
	int line;
	int level;
}lu_log_event_t;

typedef void (*lu_log_handler_t)(lu_log_event_t* event);
typedef void (*lu_log_lock_fn)(int lock, void* data);

typedef enum lu_log_level_e {
	LU_LOG_TRACE,
	LU_LOG_DEBUG,
	LU_LOG_INFO,
	LU_LOG_WARN,
	LU_LOG_ERROR,
	LU_LOG_FATAL,
}lu_log_level_t;

#define lu_log_trace(...) lu_log_log(LU_LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define lu_log_debug(...) lu_log_log(LU_LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define lu_log_info(...) lu_log_log(LU_LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define lu_log_warn(...) lu_log_log(LU_LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define lu_log_error(...) lu_log_log(LU_LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define lu_log_fatal(...) lu_log_log(LU_LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

const char* lu_log_level_to_string(lu_log_level_t level);
void lu_log_set_lock(lu_log_lock_fn lock, void* data);
void lu_log_set_level(lu_log_level_t level);
void lu_log_set_quiet(int enable);
int  lu_log_add_handler(lu_log_handler_t handler, void* data, lu_log_level_t level);
int	 lu_log_add_fp(FILE* fp, lu_log_level_t level);

void lu_log_log(lu_log_level_t level, const char* file, int line, const char* fmt, ...);

#endif /*__LU_LOG_INCLUDE_H__*/