#include "server_header.h"
#include "packet_header.h"

void packet_construct(int *user_id, int io_byte)     // 스레드간 동기화 문제 생각할 필요
{
    int rest_byte = io_byte;
    char *p = online_users[*user_id].memberInfo.exOver->buffer;
    int packet_size = 0;

    if(online_users[*user_id].memberInfo.prev_size != 0)    // 재조립을 기다리는 패킷 존재 유무
    {
        packet_size = online_users[*user_id].memberInfo.packet_buf[0];  // 재조립 패킷의 크기
    }

    while(rest_byte > 0)    // IOCP 버퍼에 남은 데이터가 없어질때까지 
    {
        if(packet_size == 0)
        {
            packet_size = p[0];
        }
            
        if(packet_size <= rest_byte + online_users[*user_id].memberInfo.prev_size)  // 남은 데이터로 패킷 완성이 가능한가?
        {
            memcpy(online_users[*user_id].memberInfo.packet_buf + online_users[*user_id].memberInfo.prev_size, p, packet_size - online_users[*user_id].memberInfo.prev_size);

            p += packet_size - online_users[*user_id].memberInfo.prev_size;
            rest_byte -= packet_size - online_users[*user_id].memberInfo.prev_size;
            packet_size = 0;

            packet_handler(user_id, online_users[*user_id].memberInfo.packet_buf);

            online_users[*user_id].memberInfo.prev_size = 0;
        }
        else    // 남은 데이터로 패킷 완성이 불가능한 경우 -> Client 구조체의 packet_buf에 저장, 포인터와 패킷 사이즈 증가
        {
            memcpy(online_users[*user_id].memberInfo.packet_buf + online_users[*user_id].memberInfo.prev_size, p, rest_byte);
            online_users[*user_id].memberInfo.prev_size += rest_byte;
            rest_byte = 0;
            p += rest_byte;
        }
    }
}

int packet_handler(int *id, char *packet_buffer)
{
    switch (packet_buffer[1])
    {
    case REGISTER:
    {
#if DEBUG
        printf("Register Request from ID %d\n", *id);
#endif

        packet_register* packet_1 = (packet_register*)packet_buffer;
        member_register(id, packet_1->id, packet_1->pw);
        break;
    }
    case LOGIN:
    {
#if DEBUG
        printf("Login Request from ID %d\n", *id);
#endif

        packet_login* packet_2 = (packet_login*)packet_buffer;
        login(id, packet_2->id, packet_2->pw);
        break;
    }
    case LOGOUT:
    {
#if DEBUG
        printf("Logout Request from ID %d\n", *id);
#endif

        logout(id);
        break;
    }
    case ENTER:
    {
#if DEBUG
        printf("Enter Request from ID %d\n", *id);
#endif
      
        packet_enter* packet_3 = (packet_enter*)packet_buffer;
        enter_room(packet_3->room_id, id);
        break;
    }
    case LEAVE:
#if DEBUG
        printf("Leave Request from ID %d\n", *id);
#endif

        quit_room(id);
        break;
    case CHAT:
    {
        packet_chat* packet_4 = (packet_chat*)packet_buffer;
        echo_message(id, packet_4->room_id, packet_4->buf);
        break;
    }
    case BLOCK:
    {
#if DEBUG
        printf("Block Request from ID %d\n", *id);
#endif
        packet_block* packet_5 = (packet_block*)packet_buffer;
        add_block_list(id, packet_5->user_name);
        break;
    }
    case MAKEROOM:
    {
#if DEBUG
        printf("MakeRoom Request from ID %d\n", *id);
#endif

        packet_makeroom* packet_6 = (packet_makeroom*)packet_buffer;
        make_room(id, packet_6->room_name);
        break;
    }
    }
    return 0;
}

/*  client에게 packet 전송
    user_id(online_users 배열 인덱스)로 소켓을 찾아 packet 전송
    return value    buf[0] - 전송한 packet의 바이트 수
                    IOCP_ERROR - WSASend 실패
*/
int packet_send(int *user_id, char *packet)
{
    
    char *buf = (char*)packet;
    member *target = &online_users[*user_id];
    LPPER_IO_DATA new;

    new = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
    new->rwMode = WRITE;
    new->wsaBuf.buf = new->buffer;
    new->wsaBuf.len = buf[0];
    memcpy(new->buffer, buf, buf[0]);

    if(WSASend(target->memberInfo.s, &new->wsaBuf, 1, NULL, 0, &new->overlapped, NULL) == SOCKET_ERROR)
    {
        if(WSAGetLastError() != WSA_IO_PENDING)
        {
            logout(user_id);
            closesocket(online_users[*user_id].memberInfo.s);
            return error_handling(IOCP_ERROR);
        }
    }

    return buf[0];
}