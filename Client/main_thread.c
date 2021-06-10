#include "client_header.h"
#include "cln_packet_header.h"

int user_main_thread(int port)
{
    int menu;
    WSADATA	wsaData;
	HANDLE hComPort;	
	SYSTEM_INFO sysInfo;
	LPPER_IO_DATA ioInfo;
    LPPER_HANDLE_DATA handleInfo;

	SOCKET hSocket;
    SOCKADDR_IN servAdr;
	int recvBytes, i, flags = 0;

    init_console();

    //mutex_lock = CreateMutex(NULL, FALSE, NULL);

    /* WSA startup */
	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        crit_error_handling("WSAStartup() error"); 
    }

	hComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	GetSystemInfo(&sysInfo);

    _beginthreadex(NULL, 0, WorkerThread, (LPVOID)hComPort, 0, NULL);
    

    /* Init connect socket */
	if((hSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED))
                == INVALID_SOCKET)
    {
        crit_error_handling("socket initialize failure");
    }

	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servAdr.sin_port = htons(port);

    ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
    memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
    ioInfo->wsaBuf.len = BUF_SIZE;
    ioInfo->wsaBuf.buf = ioInfo->buffer;
    ioInfo->rwMode = READ;

    
	if(WSAConnect(hSocket, (SOCKADDR*)&servAdr, sizeof(servAdr), NULL, NULL, NULL, NULL)
            == SOCKET_ERROR)
    {
        if((menu = WSAGetLastError()) != WSAEWOULDBLOCK)
        {
            printf("%d\n", menu);
            crit_error_handling("WSAConnect() error");
        }
    }
    handleInfo = (LPPER_HANDLE_DATA)malloc(sizeof(PER_HANDLE_DATA));
    handleInfo->hServSock = hSocket;

    CreateIoCompletionPort((HANDLE)hSocket, hComPort, (DWORD)handleInfo, 0);

    user.memberInfo.s = hSocket;
    user.memberInfo.exOver = ioInfo;

    if(WSARecv(hSocket, &(ioInfo->wsaBuf), 1, &recvBytes, &flags, &(ioInfo->overlapped), NULL) 
                == SOCKET_ERROR)
    {
        if(WSAGetLastError() != WSA_IO_PENDING)
        {
            free(ioInfo);
            /* print error "server disconneted" */
            /* return to title? exit program?   */
            return 0;
        }
    }

    menu_pointer = TITLE;

	while(true)
	{
        for(; menu_pointer == TITLE;)
        {
            init_console();
            menu = title();

            switch (menu)
            {
            case QUIT:
                closesocket(hSocket);
                return 0;

            case REGISTER:
                menu_pointer = REGISTER;
                while(true)
                {
                    CLEAR;
                    packet_register* CS_register;
                    CS_register = (packet_register*)reg_session(menu_pointer);

                    menu_pointer = TITLE;
                    if(CS_register != NULL)
                    {
                        menu_pointer = REGISTER;
                        packet_send(hSocket, CS_register);
                        while(menu_pointer == REGISTER);
                        free(CS_register);
                    }
                    if(menu_pointer == TITLE)
                    {
                        break;    
                    }
                }
                break;

            case LOGIN:
                menu_pointer = LOGIN;
                while(true)
                {
                    CLEAR;
                    packet_login* CS_login;
                    CS_login = (packet_login*)login(menu_pointer);
                    
                    menu_pointer = TITLE;

                    if(CS_login != NULL)
                    {
                        menu_pointer = LOGIN;
                        packet_send(hSocket, CS_login);
                        while(menu_pointer == LOGIN);
                        free(CS_login);
                    }
                    
                    if(menu_pointer == ROOMINFO || menu_pointer == TITLE)
                    {
                        break;
                    }
                }
                break;
            }
        }
        
        CLEAR;
        init_console();
        move_to_xy(0, 0);
        packet_request request;
        request.size = sizeof(packet_request);
        request.type = ROOMINFO;
        packet_send(hSocket, &request);
        while(menu_pointer == ROOMINFO);    // 로그인 후 서버가 방 정보 보내줄때까지 대기, 방 정보 패킷 수신하면 로비화면으로 넘어가 방 목록을 출력

        /* LOBBY 구현 필요 */

        for(; menu_pointer == LOBBY; )
        {
            printf("\n*********************\n\n");
            printf("LOGOUT < 3 >\t\t");
            printf("ENTER < 4 >\t");
            printf("MAKEROOM < 9 >\t");
            printf("\n*********************\n\n");
            printf("menu : ");
            scanf(" %d", &menu);

            switch (menu)
            {
                case LOGOUT:
                    menu_pointer = LOGOUT;
                    CLEAR;
                    packet_logout* CS_logout = (packet_logout*)malloc(sizeof(packet_logout));
                    CS_logout->id = user.user_id;
                    CS_logout->size = sizeof(packet_logout);
                    CS_logout->type = LOGOUT;
                    packet_send(hSocket, CS_logout);
                    break;
                
                case ENTER:
                    menu_pointer = ENTER;
                    CLEAR;
                    packet_enter CS_enter;
                    break;

                case MAKEROOM:
                    CLEAR;
                    packet_makeroom CS_makeroom;
                    break;
                
                default:
                    printf("잘못된 선택입니다. 다시 메뉴를 선택해주세요.\n");
                    break;
            }        
        }    
        if(menu_pointer == QUIT)
        {
            break;
        }
	}
	return 0;
}

DWORD WINAPI WorkerThread(LPVOID CompletionPortIO)      // worker thread
{
	HANDLE hComPort = (HANDLE)CompletionPortIO;
	SOCKET socket;
	DWORD bytesTrans;
	LPPER_IO_DATA ioInfo;
    LPPER_HANDLE_DATA handleInfo;
	DWORD flags = 0;
	bool GQCS;

	while(true)
	{
		GQCS = GetQueuedCompletionStatus(hComPort, &bytesTrans, (LPDWORD)&handleInfo, (LPOVERLAPPED)&ioInfo, INFINITE);
        socket = handleInfo->hServSock;

		if(!GQCS || !bytesTrans)
		{
			// 서버와의 연결 끊김
            printf("서버와의 연결이 원활하지 않습니다.\n");
            closesocket(socket);
            menu_pointer = QUIT;
			break;
		}

		if(ioInfo->rwMode == READ)	// recieved data
		{
#ifdef DEBUG
			printf("Received Packet Len: %d\n",bytesTrans);
#endif
			// 받은 패킷 조립 & 처리
            // 처리에 따른 UI draw
            packet_construct(bytesTrans, ioInfo);
			WSARecv(socket,	&(ioInfo->wsaBuf), 1, NULL, &flags, &(ioInfo->overlapped), NULL);
		}
		else if(ioInfo->rwMode == WRITE)
		{
			free(ioInfo);
		}
		
	}
		
	return 0;
}