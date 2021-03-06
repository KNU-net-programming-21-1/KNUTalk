#include "client_header.h"
#include "cln_packet_header.h"

int user_main_thread(int port)
{
    int menu;
    WSADATA	wsaData;
	HANDLE hComPort;	
	SYSTEM_INFO sysInfo;
	LPPER_IO_DATA ioInfo;
    LPPER_IO_DATA send_ioInfo;
    LPPER_HANDLE_DATA handleInfo;

	SOCKET hSocket;
    SOCKADDR_IN servAdr;
	int recvBytes, i, flags = 0;

    init_console();
    chat_pointer = 1;

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

    send_ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
    memset(&(send_ioInfo->overlapped), 0, sizeof(OVERLAPPED));
    send_ioInfo->wsaBuf.len = BUF_SIZE;
    send_ioInfo->wsaBuf.buf = send_ioInfo->buffer;
    send_ioInfo->rwMode = READ;
    
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
    user.memberInfo.sendExOver = send_ioInfo;

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
                CLEAR;
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
                        packet_send(hSocket, (char *)CS_register);
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
                        packet_send(hSocket, (char *)CS_login);
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
        
        packet_request request;
        request.size = sizeof(packet_request);
        request.type = ROOMINFO;
        menu_pointer = LOBBY;

        for(; menu_pointer == LOBBY; )
        {
            CLEAR;
            init_console();
            move_to_xy(0, 0);
            packet_send(hSocket, (char *)&request);

            menu = lobby();

            switch (menu)
            {
                case QUIT:
                    menu_pointer = LOGOUT;
                    CLEAR;
                    packet_logout* CS_logout = (packet_logout*)malloc(sizeof(packet_logout));
                    CS_logout->id = user.user_id;
                    CS_logout->size = sizeof(packet_logout);
                    CS_logout->type = LOGOUT;
                    packet_send(hSocket, (char *)CS_logout);
                    while (menu_pointer == LOGOUT);
                    break;

                case MAKEROOM + MAX_SIZE:
                    CLEAR;
                    set_console_size(COL / 2, ROW / 2);
                    menu_pointer = MAKEROOM;
                    packet_makeroom *CS_makeroom = (packet_makeroom*)malloc(sizeof(packet_makeroom));
                    CS_makeroom->size = sizeof(packet_makeroom);
                    CS_makeroom->type = MAKEROOM;
                    CS_makeroom->user_id = user.user_id;
                    
                    printf("생성할 방의 이름을 입력해주세요\n->");
                    scanf("%s", CS_makeroom->room_name);
                    packet_send(hSocket, (char *)CS_makeroom);
                    while(menu_pointer == MAKEROOM);
                    break;
                case REFRESH + MAX_SIZE:
                    break;
                default:
                    menu_pointer = ENTER;
                    CLEAR;
                    packet_enter* CS_enter = (packet_enter*)malloc(sizeof(packet_enter));
                    CS_enter->room_id = menu;
                    CS_enter->size = sizeof(packet_enter);
                    CS_enter->type = ENTER;
                    packet_send(hSocket, (char *)CS_enter);
                    while(menu_pointer == ENTER);
                    CLEAR;
                    set_console_size(100, COL);
                    break;
            }
            for (; menu_pointer == ROOM; )
            {
                packet_chat* chat_buf;
                menu_pointer = CHAT;
                chat_buf = (packet_chat*)chat_window();

                if(chat_buf != NULL)
                {
                    packet_send(hSocket, (char *)chat_buf);
                    while(menu_pointer == CHAT);
                    free(chat_buf);
                }
                else
                {   
                    if(menu_pointer == BLOCK)
                    {
                        CLEAR;
                        set_console_size(50, ROW / 2);
                        packet_block *CS_block = (packet_block*)malloc(sizeof(packet_block));
                        CS_block->size = sizeof(packet_block);
                        CS_block->type = BLOCK;

                        printf("차단할 사용자의 이름을 입력해주세요\n->");
                        scanf("%s", CS_block->user_name);
                        packet_send(hSocket, (char *)CS_block);
                        while (menu_pointer == BLOCK);
                        free(CS_block);
                        continue;
                        //free(chat_buf);
                    }
                    menu_pointer = LOBBY;
                    chat_pointer = 1;
                    packet_quit* CS_quit = (packet_quit*)malloc(sizeof(packet_quit));
                    CS_quit->room_id = user.cur_room;
                    CS_quit->size = sizeof(packet_quit);
                    CS_quit->type = LEAVE;
                    packet_send(hSocket, (char *)CS_quit);
                    room_info[user.cur_room].member_count--;
                    user.cur_room = 0;
                    free(CS_quit);
                }

                if(menu_pointer == LOBBY)
                {
                    break;
                }
            }       
        }    
        if(menu_pointer == QUIT)
        {
            break;
        }
	}
    
	free(ioInfo);
    free(send_ioInfo);
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
		GQCS = GetQueuedCompletionStatus(hComPort, &bytesTrans, (LPDWORD)&handleInfo, (LPOVERLAPPED *)&ioInfo, INFINITE);
        socket = handleInfo->hServSock;

		if(!GQCS || !bytesTrans)
		{
			// 서버와의 연결 끊김
            closesocket(socket);
            menu_pointer = QUIT;
			break;
		}

		if(ioInfo->rwMode == READ)	// recieved data
		{
#if DEBUG == 1
			printf("Received Packet Len: %d\n",bytesTrans);
#endif
			// 받은 패킷 조립 & 처리
            // 처리에 따른 UI draw
            packet_construct(bytesTrans);
			WSARecv(socket,	&(ioInfo->wsaBuf), 1, NULL, &flags, &(ioInfo->overlapped), NULL);
		}
		else if(ioInfo->rwMode == WRITE)
		{
			//free(ioInfo);
		}
		
	}
		
	return 0;
}