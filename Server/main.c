#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include "server_header.h"

int main()
{
    int port_number;

    read_from_file();
    scanf("%d", &port_number);

    accept_thread(port_number);
	return 0;
}