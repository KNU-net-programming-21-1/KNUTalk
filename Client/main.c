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
	int menu;

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
#if DEBUG == 0    
    scanf("%d", &port);
#else
	port = 65535;
#endif
    memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servAdr.sin_port = htons(port);
    
    if(connect(hSocket, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
        crit_error_handling("connect() error.");
    else
        puts("Connected......");

	/* test */
	do
	{
		printf("\n*********************\n\n");
		printf("QUIT < 0 >\t");
		printf("REGISTER < 1 >\t\t");
		printf("LOGIN < 2 >\n");
		printf("\n*********************\n\n");
		printf("menu : ");
		scanf(" %d", &menu);

		switch (menu)
		{
		case QUIT:
			closesocket(hSocket);
			exit(0);
			break;

		case REGISTER:
		{
			while (1)
			{
				packet_register CS_register;
				CS_register.type = REGISTER;
				CS_register.size = sizeof(packet_register);

				printf("ID : ");
				scanf(" %s", &CS_register.id);
				printf("PW : ");
				scanf(" %s", &CS_register.pw);

				sendBytes = send(hSocket, (const char*)&CS_register, sizeof(packet_register), 0);

				recvBytes = recv(hSocket, packet, sizeof(packet_registered), 0);
				packet_registered* SC_register = (packet_registered *)packet;

				if (SC_register->accept == true)
				{
					printf("가입에 성공하였습니다.\n");
					break;
				}
				else
				{
					printf("가입에 실패하였습니다. 다시 시도해주세요.\n");
					break;
				}
			}
			break;
		} // case REGISTER end

		case LOGIN:
		{
			while (1)
			{
				packet_login CS_login;
				CS_login.type = LOGIN;
				CS_login.size = sizeof(packet_login);
#if DEBUG == 0
				printf("ID : ");
				scanf(" %s", &CS_login.id);
				printf("PW : ");
				scanf(" %s", &CS_login.pw);
#else	// for debug mode
				strcpy(CS_login.id, "root");
				strcpy(CS_login.pw, "root");
#endif
				send(hSocket, (const char*)&CS_login, sizeof(packet_login), 0);

				recvBytes = recv(hSocket, packet, sizeof(packet_accept), 0);
				packet_accept* SC_login = (packet_accept *)packet;

				if (SC_login->accept == true)
				{
					printf("로그인에 성공하였습니다.\n");
					break;
				}
				else
				{
					printf("로그인에 실패하였습니다. 다시 시도해주세요.\n");
					break;
				}
			}
			break;
		} // case LOGIN end

		default:
			printf("잘못된 선택입니다. 다시 메뉴를 선택해주세요.\n");
			break;
		} // switch end
	} while (menu != LOGIN);

	/**********로그인 이후 *************/

	do
	{
		printf("\n*********************\n\n");
		printf("QUIT < 0 >\n");
		printf("LOGOUT < 3 >\t");
		printf("ENTER < 4 >\n");
		printf("ROOMINFO < 8 >\t");
		printf("MAKEROOM < 9 >\n");
		printf("\n*********************\n\n");
		printf("menu : ");
		scanf(" %d", &menu);

		switch (menu)
		{
		case QUIT:
			closesocket(hSocket);
			exit(0);
			break;

		case LOGOUT:
		{
			while (1) // 반복문?
			{
				packet_logout CS_logout;
				CS_logout.type = LOGOUT;
				CS_logout.size = sizeof(packet_logout);

				/*
				printf("ID : ");
				scanf(" %s", &CS_logout.id);
				*/

				sendBytes = send(hSocket, (const char*)&CS_logout, sizeof(packet_logout), 0);

				recvBytes = recv(hSocket, packet, sizeof(packet_logout_accept), 0);
				packet_logout_accept* SC_logout = (packet_logout_accept *)packet;

				if (SC_logout->accept == true)
				{
					printf("로그아웃 됐습니다.\n");
					break;
				}
				else
				{
					printf("로그아웃에 실패하였습니다. 다시 시도해주세요.\n");
					break;
				}
			}
			break;
		} // case LOGOUT end

		case ENTER:
		{
			packet_enter CS_enter;
			CS_enter.type = ENTER;
			CS_enter.size = sizeof(packet_enter);

			printf("room ID : ");
			scanf(" %d", &CS_enter.room_id);

			send(hSocket, (const char*)&CS_enter, sizeof(packet_enter), 0);

			recvBytes = recv(hSocket, packet, sizeof(packet_join), 0);
			packet_join* SC_enter = (packet_join *)packet;

			if (SC_enter->accept == true)
			{
				printf("%d 방에 입장하였습니다.\n", SC_enter->room_id);
				break;
			}
			else
			{
				printf("%d 방에 입장하지 못하였습니다.\n", SC_enter->room_id);
				break;
			}
			break;
		} // case ENTER end

		case ROOMINFO:
			break;

		case MAKEROOM:	//수정필요
		{
			packet_makeroom CS_makeroom;
			CS_makeroom.type = MAKEROOM;
			CS_makeroom.size = sizeof(packet_makeroom);
			printf("Room Name : ");
			scanf(" %s", &CS_makeroom.room_name);

			send(hSocket, (const char*)&CS_makeroom, sizeof(packet_makeroom), 0);

			recvBytes = recv(hSocket, packet, sizeof(packet_complete), 0);
			packet_complete* SC_complete = (packet_complete *)packet;

			if (SC_complete->accept == true)
			{
				printf("%d 방을 생성하였습니다.\n", SC_complete->room_id);
				break;
			}
			else
			{
				printf("%d 방을 생성하지 못하였습니다.\n", SC_complete->room_id);
				break;
			}
			break;
		} // case MAKEROOM end

		default:
			printf("잘못된 선택입니다. 다시 메뉴를 선택해주세요.\n");
			break;
		} // switch end
	} while (menu == MAKEROOM);


    closesocket(hSocket);
    WSACleanup();
    return 0;
}