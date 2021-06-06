#include "server_header.h"

int main()
{
    int port_number;

    scanf("%d", &port_number);
    init_server();
    accept_thread(port_number);
    
    fclose(mem_list);

	return 0;
}