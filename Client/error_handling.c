#include "client_header.h"

int error_handling(int err)
{
	fputs(ERROR_CODE[err], stderr);
	fputc('\n', stderr);
	return err - OFFSET;
}

void crit_error_handling(char* err_message)
{
	fputs(err_message, stderr);
	fputc('\n', stderr);
	exit(1);
}