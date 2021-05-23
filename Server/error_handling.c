#include "server_header.h"

int error_handling(int errno)
{
	fputs(ERROR_CODE[errno + OFFSET], stderr);
	fputc('\n', stderr);
	return errno;
}

void crit_error_handling(char* err_message)
{
	fputs(err_message, stderr);
	fputc('\n', stderr);
	exit(1);
}