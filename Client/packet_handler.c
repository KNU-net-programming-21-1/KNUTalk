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
    int cursor;

    switch (packet[2])
    {
    case REGISTER:
        CLEAR;
        packet_registered* packet_1 = (packet_registered*)packet;
        if (packet_1->accept == true)
        {
            printf("계정 등록에 성공하였습니다.\n");
            printf("enter키를 입력하여 타이틀로 돌아가기\n");
            while (move_cursor() != SELECT);
            menu_pointer = TITLE;
        }
        else if (packet_1->accept == DATA_DUPLICATE)
        {
            menu_pointer = DATA_DUPLICATE;
        }
        else if (packet_1->accept == DATA_FAILURE)
        {
            menu_pointer = DATA_FAILURE;
        }
        break;

    case LOGIN:
        CLEAR;
        packet_accept* packet_2 = (packet_accept*)packet;
        if (packet_2->accept == true)
        {
            user.user_id = packet_2->user_id;
            user.cur_room = 0;
            menu_pointer = ROOMINFO;
        }
        else if (packet_2->accept == false)
        {
            menu_pointer = FALSE;
        }
        break;

    case LOGOUT:
        CLEAR;
        packet_logout_accept* packet_3 = (packet_logout_accept*)packet;
        if (packet_3->accept == true)
        {
            menu_pointer == QUIT;
        }
        break;

    case ENTER:
        packet_join* packet_4 = (packet_join*)packet;
        if (packet_4->accept)
        {
            user.cur_room = packet_4->room_id;
            room_info[user.cur_room].member_count++;
            menu_pointer = ROOM;
        }
        else
        {
            menu_pointer = LOBBY;
        }
        break;

    case LEAVE:
        break;

    case CHAT:
        int i;
        packet_echo* packet_6 = (packet_chat*)packet;
        memset(chat_buffer[chat_pointer & LINE_SIZE], 0x00, sizeof(char) * LINE_SIZE);
        strcpy(chat_buffer[chat_pointer % LINE_SIZE], packet_6->user_name);
        strcat(chat_buffer[chat_pointer % LINE_SIZE], "\n>");
        strcat(chat_buffer[chat_pointer % LINE_SIZE], packet_6->buf);
        if(chat_pointer < LINE_SIZE)
        {
            print_on_xy(1, chat_pointer * 2, packet_6->user_name);
            print_on_xy(0, chat_pointer * 2 + 1, ">");
            print_on_xy(1, chat_pointer * 2 + 1, packet_6->buf);
        }
        else
        {
            for(i = 0; i < LINE_SIZE ; i++)
            {
                print_on_xy(1, i * 2, chat_buffer[(chat_pointer + i) % LINE_SIZE]);
            }
        }
        chat_pointer++;
        menu_pointer = ROOM;
        break;

    case BLOCK:
        packet_blocked* packet_7 = (packet_blocked*)packet;
        if (packet_7->accept)
        {

        }
        else
        {

        }
        //menu_pointer = 
        break;

    case ROOMINFO:
        packet_roominfo* packet_8 = (packet_roominfo*)packet;
        room_info[packet_8->room_id].member_count = packet_8->member_count;
        strcpy(room_info[packet_8->room_id].name, packet_8->room_name);
        break;

    case MAKEROOM:
        CLEAR;
        cursor = FALSE;
        packet_complete* packet_9 = (packet_complete*)packet;
        if (packet_9->accept)
        {
            strcpy(room_info[packet_9->room_id].name, packet_9->room_name);
            room_info[packet_9->room_id].member_count = 0;
            print_on_xy(0, 1, "방이 생성되었습니다.");
        }
        else
        {
            print_on_xy(0, 1, "방 생성에 실패하였습니다.");
        }
        print_on_xy(0, 3, "Press enter");
        for( ; cursor != SELECT; )
        {
            cursor = move_cursor();
        }
        menu_pointer = LOBBY;
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
    LPPER_IO_DATA new = user.memberInfo.sendExOver;

    memcpy(&size, buf, sizeof(short));

    //new = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
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