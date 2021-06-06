#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include "server_header.h"

int main()
{
    int port_number;

    scanf("%d", &port_number);
    init_server();
    accept_thread(port_number);
	return 0;
}