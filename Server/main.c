#include "server_header.h"

int main()
{
    int port_number;
#if DEBUG == 0
    scanf("%d", &port_number);
#else
    port_number = 65535;																// for debug
#endif
    init_server();
    accept_thread(port_number);
    
    fclose(mem_list);

	return 0;
}