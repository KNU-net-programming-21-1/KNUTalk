#include "client_header.h"
#include "cln_packet_header.h"

void packet_construct(int io_byte)
{
    int rest_byte = io_byte;
    char *p = user.memberInfo.exOver->buffer;
    int packet_size = 0;
    short size, prev_size;

    memcpy(&prev_size, user.memberInfo.packet_buf, sizeof(short));

    if(user.memberInfo.prev_size != 0)
    {
        packet_size = prev_size;
    }

    while(rest_byte > 0)
    {
        memcpy(&size, p, sizeof(short));

        if(packet_size == 0)
        {
            packet_size = size;
        }
            
        if(packet_size <= rest_byte + user.memberInfo.prev_size)
        {
            memcpy(user.memberInfo.packet_buf + user.memberInfo.prev_size, p, packet_size - user.memberInfo.prev_size);

            p += packet_size - user.memberInfo.prev_size;
            rest_byte -= packet_size - user.memberInfo.prev_size;
            packet_size = 0;

            packet_handler(user.memberInfo.packet_buf);

            user.memberInfo.prev_size = 0;
        }
        else
        {
            memcpy(user.memberInfo.packet_buf + user.memberInfo.prev_size, p, rest_byte);
            user.memberInfo.prev_size += rest_byte;
            rest_byte = 0;
            p += rest_byte;
        }
    }
}

int packet_handler(char *packet)
{
    switch (packet[2])
    {
    case REGISTER:
        CLEAR;
        packet_registered* packet_1 = (packet_registered*)packet;
        if(packet_1->accept == true)
        {
            printf("계정 등록에 성공하였습니다.\n");
            printf("enter키를 입력하여 타이틀로 돌아가기\n");
            while(move_cursor() != SELECT);
            menu_pointer = TITLE;
        }
        else if(packet_1->accept == DATA_DUPLICATE)
        {
            menu_pointer = DATA_DUPLICATE;
        }
        else if(packet_1->accept == DATA_FAILURE)
        {
            menu_pointer = DATA_FAILURE;
        }
        break;

    case LOGIN:
        CLEAR;
        packet_accept* packet_2 = (packet_accept*)packet;
        if(packet_2->accept == true)
        {
            user.user_id = packet_2->user_id;
            menu_pointer = ROOMINFO;
        }
        else if(packet_2->accept == false)
        {
            menu_pointer = FALSE;
        }
        break;
    
    case LOGOUT:
        CLEAR;
        packet_logout_accept* packet_3 = (packet_logout_accept*)packet;
        if(packet_3->accept == true)
        {
            menu_pointer == LOGOUT;
        }
        break;
    
    case ENTER:
        break;
    
    case LEAVE:

        break;

    case CHAT:
        break;

    case BLOCK:
        break;
    
    case ROOMINFO:
        CLEAR;
        int i;
        packet_roominfo* packet_8 = (packet_roominfo*)packet;
        for(i = 0; i < MAX_SIZE; i++)
        {
            if(packet_8->room_member[i] != -1)
            {
                printf("ROOM ID : %d || %d/%d\n", i, packet_8->room_member[i], MAX_SIZE);
            }
        }
        menu_pointer = LOBBY;
        break;
    
    case MAKEROOM:
        break;
    }
    return 0;

}

int packet_send(SOCKET s, char* packet)
{
#if DEBUG == 1
    puts("PACKET SEND");
#endif
    char* buf = (char*)packet;
    short size;
    LPPER_IO_DATA new;

    memcpy(&size, buf, sizeof(short));

    new = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
    new->rwMode = WRITE;
    new->wsaBuf.buf = new->buffer;
    new->wsaBuf.len = size;

    memset(&new->overlapped, 0x00, sizeof(OVERLAPPED));
    memcpy(new->buffer, buf, size);

    if (WSASend(s, &new->wsaBuf, 1, NULL, 0, &new->overlapped, NULL) == SOCKET_ERROR)
    {
        if (WSAGetLastError() != WSA_IO_PENDING)
        {
            closesocket(s);
            return error_handling(IOCP_ERROR + OFFSET);
        }
    }

    return buf[0];
}