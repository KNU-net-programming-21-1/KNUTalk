#include "server_header.h"


int accept_thread(int port)
{
    WSADATA	wsaData;
	HANDLE hComPort;	
	SYSTEM_INFO sysInfo;
	LPPER_HANDLE_DATA handleInfo;

	SOCKET hServSock;
	SOCKADDR_IN servAdr;
	int recvBytes, i, flags=0;
	int empty_slot;						// online_users 배열의 빈 부분 탐색용

    /* WSA startup */
	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        crit_error_handling("WSAStartup() error"); 
    }

	hComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	GetSystemInfo(&sysInfo);

    /* Add workers to thread pool */
	for(i = 0; i < sysInfo.dwNumberOfProcessors; i++)
    {
        _beginthreadex(NULL, 0, WorkerThread, (LPVOID)hComPort, 0, NULL);
    }
    /* Init accept socket */
	if((hServSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED))
                == INVALID_SOCKET)
    {
        crit_error_handling("socket initialize failure");
    }

	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons(port);

    /* Set socket & create listen queue */
	if(bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr))
            == SOCKET_ERROR)
    {
        crit_error_handling("bind() error");
    }
	if(listen(hServSock, 5) == SOCKET_ERROR)
    {
        crit_error_handling("listen() error");
    }
	
	/* accept loop */
	while(true)
	{	
		SOCKET hClntSock;
		SOCKADDR_IN clntAdr;		
		int addrLen = sizeof(clntAdr);
		
		hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &addrLen);	

		for(i = 0; i < MAX_SIZE; i++)
		{
			empty_slot = LIMIT_REACHED;
			if(online_users[i].is_online == false)
			{
				empty_slot = i;	// online_users 배열의 비어있는 부분에 새 연결 할당
				break;			// 최대 동접자 수 초과할 경우...일단 보류 -> 대기 알리는 패킷전송, 로그아웃으로 빈자리 생길경우 스레드에 알려주는 기능, 메인 스레드는 새 연결 계속 받아서 대기 패킷 전송 함수 호출해야 함
			}
		}
		  
		if(empty_slot == LIMIT_REACHED)
		{
			/* send limit reached error packet to client */
			shutdown(hClntSock, SD_SEND);
			closesocket(hClntSock);
		}
		else
		{
			handleInfo = (LPPER_HANDLE_DATA)malloc(sizeof(PER_HANDLE_DATA));		
			handleInfo->hClntSock = hClntSock;
			memcpy(&(handleInfo->clntAdr), &clntAdr, addrLen);
			handleInfo->user_index = empty_slot;

			CreateIoCompletionPort((HANDLE)hClntSock, hComPort, (DWORD)handleInfo, 0);
			
			memset(&(online_users[empty_slot].memberInfo.exOver->overlapped), 0, sizeof(OVERLAPPED));
			online_users[empty_slot].memberInfo.exOver->wsaBuf.len = BUF_SIZE;
			online_users[empty_slot].memberInfo.exOver->wsaBuf.buf = online_users[empty_slot].memberInfo.exOver->buffer;
			online_users[empty_slot].memberInfo.exOver->rwMode = READ;
			
			memset(&(online_users[empty_slot].memberInfo.sendExOver->overlapped), 0, sizeof(OVERLAPPED));
			online_users[empty_slot].memberInfo.sendExOver->wsaBuf.len = BUF_SIZE;
			online_users[empty_slot].memberInfo.sendExOver->wsaBuf.buf = online_users[empty_slot].memberInfo.sendExOver->buffer;
			online_users[empty_slot].memberInfo.sendExOver->rwMode = WRITE;
			
			online_users[empty_slot].memberInfo.s = hClntSock;
			online_users[empty_slot].is_online = true;
			WSARecv(handleInfo->hClntSock,	&(online_users[empty_slot].memberInfo.exOver->wsaBuf), 1, &recvBytes, &flags, &(online_users[empty_slot].memberInfo.exOver->overlapped), NULL);
		}
	}
	return 0;
}

DWORD WINAPI WorkerThread(LPVOID CompletionPortIO)      // worker thread
{
	HANDLE hComPort = (HANDLE)CompletionPortIO;	
	WSADATA	wsaData;
	SOCKET socket;
	DWORD bytesTrans;
	LPPER_IO_DATA ioInfo;
	LPPER_HANDLE_DATA handleInfo;
	DWORD flags = 0;
	int error;
	bool GQCS;

	while(true)
	{
		GQCS = GetQueuedCompletionStatus(hComPort, &bytesTrans, (LPDWORD)&handleInfo, (LPOVERLAPPED)&ioInfo, INFINITE);
		socket = handleInfo->hClntSock;

		if(!GQCS || !bytesTrans)
		{
			if(!GQCS)	
			{
				error_handling(IOCP_ERROR + OFFSET); 
				if (online_users[handleInfo->user_index].is_online)
				{
					logout(handleInfo->user_index);	// client가 로그아웃 패킷 전송 없이 종료되었을 경우에도 유저 로그아웃 처리
				}
				closesocket(socket);
			}
			continue;
		}

		if(ioInfo->rwMode == READ)	// recieved data
		{
#ifdef DEBUG
			printf("Received Packet Len: %d\n",bytesTrans);
#endif
			packet_construct(handleInfo->user_index, bytesTrans);	// 받은 패킷 조립 & 처리

			WSARecv(socket,	&(ioInfo->wsaBuf), 1, NULL, &flags, &(ioInfo->overlapped), NULL);	
		}
		else if(ioInfo->rwMode == WRITE)
		{
			//free(ioInfo);
		}
	}
		
	return 0;
}