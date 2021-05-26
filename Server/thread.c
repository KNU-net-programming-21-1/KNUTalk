#include "server_header.h"
#include <process.h>
#include <windows.h>
#define READ	3
#define	WRITE	5

typedef struct    // socket info
{
	SOCKET hClntSock;
	SOCKADDR_IN clntAdr;
} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

typedef struct    // buffer info
{
	OVERLAPPED overlapped;
	WSABUF wsaBuf;
	char buffer[BUF_SIZE];
	int rwMode;    // READ or WRITE
} PER_IO_DATA, *LPPER_IO_DATA;

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
	servAdr.sin_addr.s_addr = htonl("127.0.0.1");
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
		int addrLen=sizeof(clntAdr);
		
		hClntSock=accept(hServSock, (SOCKADDR*)&clntAdr, &addrLen);		  

		handleInfo=(LPPER_HANDLE_DATA)malloc(sizeof(PER_HANDLE_DATA));		
		handleInfo->hClntSock=hClntSock;
		memcpy(&(handleInfo->clntAdr), &clntAdr, addrLen);

		CreateIoCompletionPort((HANDLE)hClntSock, hComPort, (DWORD)handleInfo, 0);
		
		ioInfo=(LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
		memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));		
		ioInfo->wsaBuf.len=BUF_SIZE;
		ioInfo->wsaBuf.buf=ioInfo->buffer;
		ioInfo->rwMode=READ;

		WSARecv(handleInfo->hClntSock,	&(ioInfo->wsaBuf),	
			1, &recvBytes, &flags, &(ioInfo->overlapped), NULL);			
	}
	return 0;
}

DWORD WINAPI WorkerThread(LPVOID CompletionPortIO)      // worker thread
{
	/*
		클라이언트 - 서버 데이터 송수신,
		클라이언트 - 서버 간 발생 이벤트 핸들,
		이벤트 처리 위한 함수 호출 등 실제 기능 구현부
	*/

	return 0;
}