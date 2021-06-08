#include "client_header.h"
#include "cln_packet_header.h"


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
	int cur_room_num;

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

			if (SC_register->accept == true) printf("���Կ� �����Ͽ����ϴ�.\n");
			else printf("���Կ� �����Ͽ����ϴ�. �ٽ� �õ����ּ���.\n");
			break;
		} // case REGISTER end

		case LOGIN:
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

			if (SC_login->accept == true) printf("�α��ο� �����Ͽ����ϴ�.\n");
			else printf("�α��ο� �����Ͽ����ϴ�. �ٽ� �õ����ּ���.\n");
			break;
		} // case LOGIN end

		default:
			printf("�߸��� �����Դϴ�. �ٽ� �޴��� �������ּ���.\n");
			break;
		} // switch end
	} while (menu != LOGIN);

	/**********�α��� ���� *************/

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

			if (SC_logout->accept == true) printf("�α׾ƿ� �ƽ��ϴ�.\n");
			else printf("�α׾ƿ��� �����Ͽ����ϴ�. �ٽ� �õ����ּ���.\n");
			break;
		} // case LOGOUT end

		case ENTER:
		{
			packet_enter CS_enter;
			CS_enter.type = ENTER;
			CS_enter.size = sizeof(packet_enter);

			printf("room ID : ");
			scanf(" %d", &cur_room_num); //�ӽ�
			CS_enter.room_id = cur_room_num;

			send(hSocket, (const char*)&CS_enter, sizeof(packet_enter), 0);

			recvBytes = recv(hSocket, packet, sizeof(packet_join), 0);
			packet_join* SC_enter = (packet_join *)packet;

			if (SC_enter->accept == true) printf("%d �濡 �����Ͽ����ϴ�.\n", SC_enter->room_id);
			else printf("%d �濡 �������� ���Ͽ����ϴ�.\n", SC_enter->room_id);
			break;
		} // case ENTER end

		case ROOMINFO:
			break;

		case MAKEROOM:	//�����ʿ�
		{
			packet_makeroom CS_makeroom;
			CS_makeroom.type = MAKEROOM;
			CS_makeroom.size = sizeof(packet_makeroom);
			printf("Room Name : ");
			scanf(" %s", &CS_makeroom.room_name);

			send(hSocket, (const char*)&CS_makeroom, sizeof(packet_makeroom), 0);

			recvBytes = recv(hSocket, packet, sizeof(packet_complete), 0);
			packet_complete* SC_complete = (packet_complete *)packet;

			if (SC_complete->accept == true) printf("%d ���� �����Ͽ����ϴ�.\n", SC_complete->room_id);
			else
			{
				printf("%d ���� �������� ���Ͽ����ϴ�.\n", SC_complete->room_id);
				continue;
			}
			break;
		} // case MAKEROOM end

		default:
			printf("�߸��� �����Դϴ�. �ٽ� �޴��� �������ּ���.\n");
			break;
		} // switch end
	} while (menu == MAKEROOM);

	/********** �� ���� ���� *************/
	/*
	�ϴ��� �⺻������ CHAT ���, �ٸ� ��Ŷ(ex. block, leave ��) ���� �ÿ� /num���� �Է�

	���� ->
	�渶�� ť ����?
	ä�� ���� �ٸ� �̿��ڰ� ���� �� packet_leave �޾ƾ� ��
	cur_room�� �ƴ� �濡�� ä���� ���� �� �ִ���? (�ӽ� �׽�Ʈ������ cur_room_num ���)
	*/

	printf("\n*********************\n\n");
	printf("QUIT < /0 >\t");
	printf("LOGOUT < /3 >\n");
	printf("LEAVE < /5 >\t");
	printf("BLOCK < /7 >\n");
	printf("\n*********************\n\n");

	while (1)
	{
		packet_chat CS_chat;
		packet_echo* SC_echo;
		CS_chat.type = CHAT;
		CS_chat.size = sizeof(packet_chat);
		char temp_buf[MAX_SIZE];

		printf("Sending message : ");
		gets_s(temp_buf, MAX_SIZE);

		/************* ä�� �̿� *************/
		if (!strcmp(temp_buf, "/0"))
		{
			closesocket(hSocket);
			exit(0);
			break;
		} // if QUIT end

		else if (!strcmp(temp_buf, "/3"))
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

			if (SC_logout->accept == true) printf("�α׾ƿ� �ƽ��ϴ�.\n");
			else printf("�α׾ƿ��� �����Ͽ����ϴ�. �ٽ� �õ����ּ���.\n");
			break;
		} // else if LOGOUT end

		else if (!strcmp(temp_buf, "/5"))
		{
			packet_quit CS_leave;
			CS_leave.type = LEAVE;
			CS_leave.size = sizeof(packet_quit);

			printf("������ �� ��ȣ�� �Է����ּ���. : ");
			scanf(" %d", &CS_leave.room_id);
			send(hSocket, (const char*)&CS_chat, sizeof(packet_quit), 0);

			printf("%d �濡�� �����Ͽ����ϴ�.\n", CS_leave.room_id);	// ���� �ʿ�
			break;
			/*
			���� ���� �ٸ� ����ڵ��� �޴� ��Ŷ

			recvBytes = recv(hSocket, packet, sizeof(packet_leave), 0);
			packet_leave* SC_leave = (packet_leave *)packet;
			*/
		} // else if LEAVE end

		else if (!strcmp(temp_buf, "/7"))
		{
			char temp_name[MAX_SIZE];
			packet_block CS_block;
			CS_block.type = BLOCK;
			CS_block.size = sizeof(packet_block);

			printf("������ ������� �̸��� �Է����ּ���. : ");
			scanf(" %s", &temp_name);
			strcpy(CS_block.user_name, temp_name);
			send(hSocket, (const char*)&CS_block, sizeof(packet_block), 0);

			recvBytes = recv(hSocket, packet, sizeof(packet_blocked), 0);
			packet_blocked* SC_block = (packet_blocked *)packet;

			if (SC_block->accept == true) printf("'%s'��(��)�� �����Ͽ����ϴ�.\n\n", temp_name);
			else printf("�������� �ʴ� ������Դϴ�.\n\n");
			continue;
		} // else if BLOCK end

		strcpy(CS_chat.buf, temp_buf);
		CS_chat.room_id = cur_room_num; // �ӽ�
		send(hSocket, (const char*)&CS_chat, sizeof(packet_chat), 0);

		recvBytes = recv(hSocket, packet, sizeof(packet_echo), 0);
		SC_echo = (packet_echo *)packet;

		printf("[%s] : %s\n\n", SC_echo->user_name, SC_echo->buf);
	}

    closesocket(hSocket);
    WSACleanup();
    return 0;
}