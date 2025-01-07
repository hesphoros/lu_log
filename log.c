#include "log.h"

#define LU_LOG_USE_COLOR
#define MAX_CALLBACKS 32

typedef struct lu_log_callback_s {
	lu_log_handler_t handler;
	void* data;
	lu_log_level_t level;
}lu_log_callback_t;

static  struct {
	void* data;
	lu_log_lock_fn lock;
	lu_log_level_t level;
	int quiet;
	lu_log_callback_t callbacks[MAX_CALLBACKS];
}lu_log_config_t;

static const char* lu_log_level_strings[] = {
	"TRACE",
	"DEBUG",
	"INFO",
	"WARN",
	"ERROR",
	"FATAL",
};

#ifdef LU_LOG_USE_COLOR
static const char* lu_level_colors[] = {
	"\x1b[94m",  // TRACE
	"\x1b[36m",  // DEBUG
	"\x1b[32m",  // INFO
	"\x1b[33m",  // WARN
	"\x1b[31m",  // ERROR
	"\x1b[35m",  // FATAL
};
#endif

// 输出到标准输出的回调
static void lu_stdout_callback(lu_log_event_t* log_event) {
	char buf[16];
	buf[strftime(buf, sizeof(buf), "%H:%M:%S", log_event->time_info)] = '\0';
#ifdef LU_LOG_USE_COLOR
	if (isatty(fileno(log_event->data))) {
		fprintf(
			log_event->data, "%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ",
			buf, lu_level_colors[log_event->level], lu_log_level_strings[log_event->level],
			log_event->file, log_event->line);
	}
	else {
		fprintf(
			log_event->data, "%s %-5s %s:%d: ",
			buf, lu_log_level_strings[log_event->level], log_event->file, log_event->line);
	}
#else
	fprintf(
		log_event->data, "%s %-5s %s:%d: ",
		buf, lu_log_level_strings[log_event->level], log_event->file, log_event->line);
#endif
	vfprintf(log_event->data, log_event->fmt, log_event->ap);
	fprintf(log_event->data, "\n");
	fflush(log_event->data);
}

static void lu_file_callback(lu_log_event_t* log_event) {
	char buf[64];
	buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", log_event->time_info)] = '\0';
	fprintf(
		log_event->data, "%s %-5s %s:%d: ",
		buf, lu_log_level_strings[log_event->level], log_event->file, log_event->line);
	vfprintf(log_event->data, log_event->fmt, log_event->ap);
	fprintf(log_event->data, "\n");
	fflush(log_event->data);
}

static void lu_lock(void) {
	if (lu_log_config_t.lock) { lu_log_config_t.lock(1, lu_log_config_t.data); }
}

static void lu_unlock(void) {
	if (lu_log_config_t.lock) { lu_log_config_t.lock(0, lu_log_config_t.data); }
}

const char* lu_log_level_to_string(lu_log_level_t level)
{
	return lu_log_level_strings[level];
}

void lu_log_set_lock(lu_log_lock_fn lock, void* data)
{
	lu_log_config_t.lock = lock;
	lu_log_config_t.data = data;
}

void lu_log_set_level(lu_log_level_t level)
{
	lu_log_config_t.level = level;
}

void lu_log_set_quiet(int enable)
{
	lu_log_config_t.quiet = enable;
}

int lu_log_add_handler(lu_log_handler_t handler, void* data, lu_log_level_t level)
{
	for (size_t i = 0; i < MAX_CALLBACKS; i++)
	{
		if (!lu_log_config_t.callbacks[i].handler) {
			lu_log_config_t.callbacks[i] = (lu_log_callback_t){ handler,data,level };
			return 0;
		}
	}
	return -1;
}

int lu_log_add_fp(FILE* fp, lu_log_level_t level)
{
	return lu_log_add_handler(lu_file_callback, fp, level);
}

static void lu_init_event(lu_log_event_t* log_event, void* data) {
	if (!log_event->time_info) {
		time_t t = time(NULL);
		log_event->time_info = localtime(&t);
	}
	log_event->data = data;
}

void lu_log_log(lu_log_level_t level, const char* file, int line, const char* fmt, ...)
{
	lu_log_event_t log_event = {
		.fmt = fmt,
		.file = file,
		.line = line,
		.level = level,
	};

	lu_lock();
	if (!lu_log_config_t.quiet && level >= lu_log_config_t.level) {
		lu_init_event(&log_event, stderr);
		va_start(log_event.ap, fmt);
		lu_stdout_callback(&log_event);
		va_end(log_event.ap);
	}

	for (size_t i = 0; i <= MAX_CALLBACKS && lu_log_config_t.callbacks[i].handler; i++) {
		lu_log_callback_t* cb = &lu_log_config_t.callbacks[i];
		if (level >= cb->level) {
			lu_init_event(&log_event, cb->data);
			va_start(log_event.ap, fmt);
			cb->handler(&log_event);
			va_end(log_event.ap);
		}
	}
	lu_unlock();
}