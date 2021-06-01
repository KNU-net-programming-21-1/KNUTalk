#include "server_header.h"
#include <process.h>
#include <windows.h>
#define READ	3
#define	WRITE	5

int accept_thread(int port)
{
    WSADATA	wsaData;
	HANDLE hComPort;	
	SYSTEM_INFO sysInfo;
	LPPER_IO_DATA ioInfo;
	LPPER_HANDLE_DATA handleInfo;

	SOCKET hServSock;
	SOCKADDR_IN servAdr;
	int recvBytes, i, flags=0;
	int online_num = 0;

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
	servAdr.sin_port = htons(atoi(port));

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

		handleInfo = (LPPER_HANDLE_DATA)malloc(sizeof(PER_HANDLE_DATA));		
		handleInfo->hClntSock = hClntSock;
		memcpy(&(handleInfo->clntAdr), &clntAdr, addrLen);
		handleInfo->user_index = online_num++;

		CreateIoCompletionPort((HANDLE)hClntSock, hComPort, (DWORD)handleInfo, 0);
		
		ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
		memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));		
		ioInfo->wsaBuf.len = BUF_SIZE;
		ioInfo->wsaBuf.buf = ioInfo->buffer;
		ioInfo->rwMode = READ;
		
		WSARecv(handleInfo->hClntSock,	&(ioInfo->wsaBuf), 1, &recvBytes, &flags, &(ioInfo->overlapped), NULL);	
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
	bool GQCS;

	while(true)
	{
		GQCS = GetQueuedCompletionStatus(hComPort, &bytesTrans, (LPDWORD)&handleInfo, (LPOVERLAPPED)&ioInfo, INFINITE);
		if(GQCS || bytesTrans == 0)
		{
			if(bytesTrans != 0)	
			{
				error_handling(IOCP_ERROR);
			}

			closesocket(handleInfo->hClntSock);	// client가 로그아웃 패킷 전송 없이 종료되었을 경우 유저 로그아웃 처리
			logout(handleInfo->user_index);
			continue;
		}

		if(ioInfo->rwMode == READ)	// recieved data
		{
			packet_construct(handleInfo->user_index, bytesTrans);	// 받은 패킷 조립 & 처리
		}
		
		WSARecv(socket,	&(ioInfo->wsaBuf), 1, NULL, &flags, &(ioInfo->overlapped), NULL);	
	}
		
	return 0;
}