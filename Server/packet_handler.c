#include "server_header.h"
#include "packet_header.h"

void packet_construct(int user_id, int io_byte)     // 스레드간 동기화 문제 생각할 필요
{
    int rest_byte = io_byte;
    char *p = online_users[user_id].memberInfo.exOver->buffer;
    int packet_size = 0;
    short size, prev_size;

    memcpy(&prev_size, online_users[user_id].memberInfo.packet_buf, sizeof(short));

    if(online_users[user_id].memberInfo.prev_size != 0)    // 재조립을 기다리는 패킷 존재 유무
    {
        packet_size = prev_size;  // 재조립 패킷의 크기
    }

    while(rest_byte > 0)    // IOCP 버퍼에 남은 데이터가 없어질때까지 
    {
        memcpy(&size, p, sizeof(short));

        if(packet_size == 0)
        {
            packet_size = size;
        }
            
        if(packet_size <= rest_byte + online_users[user_id].memberInfo.prev_size)  // 남은 데이터로 패킷 완성이 가능한가?
        {
            memcpy(online_users[user_id].memberInfo.packet_buf + online_users[user_id].memberInfo.prev_size, p, packet_size - online_users[user_id].memberInfo.prev_size);

            p += packet_size - online_users[user_id].memberInfo.prev_size;
            rest_byte -= packet_size - online_users[user_id].memberInfo.prev_size;
            packet_size = 0;

            packet_handler(user_id, online_users[user_id].memberInfo.packet_buf);

            online_users[user_id].memberInfo.prev_size = 0;
        }
        else    // 남은 데이터로 패킷 완성이 불가능한 경우 -> Client 구조체의 packet_buf에 저장, 포인터와 패킷 사이즈 증가
        {
            memcpy(online_users[user_id].memberInfo.packet_buf + online_users[user_id].memberInfo.prev_size, p, rest_byte);
            online_users[user_id].memberInfo.prev_size += rest_byte;
            rest_byte = 0;
            p += rest_byte;
        }
    }
}

int packet_handler(int user_id, char *packet_buffer)
{
    switch (packet_buffer[2])
    {
    case REGISTER:
#if DEBUG
        printf("Register Request from ID %d\n", user_id);
#endif
        packet_register* packet_1 = (packet_register*)packet_buffer;
        member_register(user_id, packet_1->id, packet_1->pw);
        break;

    case LOGIN:
#if DEBUG
        printf("Login Request from ID %d\n", user_id);
#endif
        packet_login* packet_2 = (packet_login*)packet_buffer;
        login(user_id, packet_2->id, packet_2->pw);
        break;
    
    case LOGOUT:
#if DEBUG
        printf("Logout Request from ID %d\n", user_id);
#endif
        packet_logout_accept packet_3;
        logout(user_id);
        packet_3.accept = true;
        packet_3.size = sizeof(packet_logout_accept);
        packet_3.type = LOGOUT;
        packet_send(user_id, (char *)&packet_3);
        shutdown(online_users[user_id].memberInfo.s, SD_SEND);
        break;
    
    case ENTER:
#if DEBUG
        printf("Enter Request from ID %d\n", user_id);
#endif
        packet_enter* packet_4 = (packet_enter*)packet_buffer;
        enter_room(packet_4->room_id, user_id);
        break;
    
    case LEAVE:
#if DEBUG
        printf("Leave Request from ID %d\n", user_id);
#endif
        packet_quit* packet_5 = (packet_quit*)packet_buffer;
        char buf[MAX_SIZE];
        quit_room(packet_5->room_id, user_id);
        strcpy(buf, online_users[user_id].id);
        strcat(buf, "님이 퇴장하였습니다.");
        echo_message(user_id, packet_5->room_id, buf);
        break;

    case CHAT:
#if DEBUG
        printf("Echo Request from ID %d\n", user_id);
#endif
        packet_chat* packet_6 = (packet_chat*)packet_buffer;
        echo_message(user_id, packet_6->room_id, packet_6->buf);
        break;

    case BLOCK:
#if DEBUG
        printf("Block Request from ID %d\n", user_id);
#endif
        packet_block* packet_7 = (packet_block*)packet_buffer;
        add_block_list(user_id, packet_7->user_name);
        break;
    case ROOMINFO:
#if DEBUG
        printf("RoomInfo Request from ID %d\n", user_id);
#endif
        room_info_request(user_id);
        break;

    case MAKEROOM:
#if DEBUG
        printf("MakeRoom Request from ID %d\n", user_id);
#endif
        packet_makeroom* packet_9 = (packet_makeroom*)packet_buffer;
        make_room(user_id, packet_9->room_name);
        break;
    
    }
    return 0;
}

/*  client에게 packet 전송
    user_id(online_users 배열 인덱스)로 소켓을 찾아 packet 전송
    return value    buf[0] - 전송한 packet의 바이트 수
                    IOCP_ERROR - WSASend 실패
*/
int packet_send(int user_id, char *packet)
{
#if DEBUG == 1
    puts("PACKET SEND");
#endif
    char *buf = (char*)packet;
    member *target = &online_users[user_id];
    short size;
    LPPER_IO_DATA new = online_users[user_id].memberInfo.sendExOver;

    memcpy(&size, buf, sizeof(short));

    /*new = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));*/

    memset(&new->overlapped, 0x00, sizeof(OVERLAPPED));
    memcpy(new->buffer, buf, size);
    new->rwMode = WRITE;
    new->wsaBuf.buf = new->buffer;
    new->wsaBuf.len = size;

    if (WSASend(target->memberInfo.s, &(new->wsaBuf), 1, NULL, 0, &(new->overlapped), NULL) == SOCKET_ERROR)
    {
        if (WSAGetLastError() != WSA_IO_PENDING)
        {
            return error_handling(IOCP_ERROR + OFFSET);
        }
    }

    return buf[0];
}