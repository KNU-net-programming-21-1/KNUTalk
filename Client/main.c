#include "client_header.h"


int main()
{
    WSADATA wsaData;
    SOCKET hSocket;
    SOCKADDR_IN servAdr;
    
    char packet[PACKET_SIZE];
    char id[ID_SIZE];
    char pw[PW_SIZE];
    int sendBytes, recvBytes;
    int port;

    if(WSAStartup(MAKEWORD(2, 2), &wsaData)!=0)
    {
        crit_error_handling("socket initialize failure");
        exit(1);
    }
    
    hSocket = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if(hSocket==INVALID_SOCKET)
    {
        crit_error_handling("socket() error.");
    }    
    
    scanf("%d", &port);
    memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servAdr.sin_port = htons(port);
    
    if(connect(hSocket, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
        crit_error_handling("connect() error.");
    else
        puts("Connected......");

    /* Login test */
    while(1){
        printf("Login Test\n");
        printf("ID : ");
        scanf("%s",id);
        printf("PW : ");
        scanf("%s",pw);
        
        packet_login login;
        login.size = sizeof(packet_login);
        login.type = LOGIN;
        strcpy(login.id, id);
        strcpy(login.pw, pw);

        memcpy(packet, &login, login.size);

        sendBytes = send(hSocket, packet, sizeof(packet_login), 0);
        printf("Send data len : %d\n", sendBytes);


        recvBytes = recv(hSocket, packet, sizeof(packet_accept), 0);

        if (*(packet + 2) == TRUE) {
            printf("Login Success!\n");
            break;
        }
    }


    closesocket(hSocket);
    WSACleanup();
    return 0;
}