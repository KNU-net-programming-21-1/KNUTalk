#include "client_header.h"
#include "cln_packet_header.h"
#include <process.h>
#include <windows.h>

int user_main_thread(int port)
{
    WSADATA	wsaData;
	HANDLE hComPort;	
	SYSTEM_INFO sysInfo;
	LPPER_IO_DATA ioInfo;

	SOCKET hSocket;
    SOCKADDR_IN servAdr;
	int recvBytes, i, flags = 0;

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

    /* Init connect socket */
	if((hSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED))
                == INVALID_SOCKET)
    {
        crit_error_handling("socket initialize failure");
    }

	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl("127.0.0.1");
	servAdr.sin_port = htons(port);

    ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
    memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));		
    ioInfo->wsaBuf.len = BUF_SIZE;
    ioInfo->wsaBuf.buf = ioInfo->buffer;
    ioInfo->rwMode = READ;

    CreateIoCompletionPort((HANDLE)hSocket, hComPort, 0, 0);

	if(WSAConnect(hSocket, (SOCKADDR*)&servAdr, sizeof(servAdr), NULL, NULL, NULL, NULL)
            == SOCKET_ERROR)
    {
        if(WSAGetLastError() != WSAEWOULDBLOCK)
        {
            crit_error_handling("WSAConnect() error");
        }
    }

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

	while(true)
	{
        // title 화면 출력

        /* main loop */
        /*   ...     */     // => 유저에게 입력받아 서버에 데이터 전송
        /* main loop */

        // WSASend()
        // malloc send ioInfo & new ioInfo->rwMode = WRITE -> need to make packet_send function for client
        // new ioInfo will free() in WorkerThread after IO complete
	}

	return 0;
}

DWORD WINAPI WorkerThread(LPVOID CompletionPortIO)      // worker thread
{
	HANDLE hComPort = (HANDLE)CompletionPortIO;
	SOCKET socket;
	DWORD bytesTrans;
	LPPER_IO_DATA ioInfo;
    ULONG handleInfo;
	DWORD flags = 0;
	bool GQCS;

	while(true)
	{
		GQCS = GetQueuedCompletionStatus(hComPort, &bytesTrans, &handleInfo, (LPOVERLAPPED)&ioInfo, INFINITE);
		
		if(!GQCS && !bytesTrans)
		{
			closesocket(socket);    // 서버와의 연결 끊김
			break;
		}

		if(ioInfo->rwMode == READ)	// recieved data
		{
#ifdef DEBUG
			printf("Received Packet Len: %d\n",bytesTrans);
#endif
			// 받은 패킷 조립 & 처리
            // 처리에 따른 UI draw

			WSARecv(socket,	&(ioInfo->wsaBuf), 1, NULL, &flags, &(ioInfo->overlapped), NULL);
		}
		else if(ioInfo->rwMode == WRITE)
		{
			free(ioInfo);
		}
		
	}
		
	return 0;
}