#define LU_LOG_USE_COLOR
#include "log.h"

int main() {
	// ����־�ļ�
	FILE* logfile = fopen("logfile.txt", "a");
	if (logfile == NULL) {
		perror("Failed to open log file");
		return 1;
	}

	// ���ļ�ָ����ӵ���־���У����������־����Ϊ LOG_INFO
	lu_log_add_fp(logfile, LU_LOG_INFO);

	// ������־����Ϊ LOG_DEBUG
	lu_log_set_level(LU_LOG_DEBUG);
	lu_log_set_quiet(1);
	// ������־���
	lu_log_trace("This is a trace message");
	lu_log_debug("This is a debug message");
	lu_log_info("This is an info message");
	lu_log_warn("This is a warning message");
	lu_log_error("This is an error message");
	lu_log_fatal("This is a fatal message");

	// �ر��ļ�
	fclose(logfile);

	return 0;
}