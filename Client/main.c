#include "client_header.h"
#include "cln_packet_header.h"


int main(void)
{
	int port;

#if DEBUG == 0    
	scanf("%d", &port);
#else
	port = 65535;
#endif

	user_main_thread(port);
	return 0;
}