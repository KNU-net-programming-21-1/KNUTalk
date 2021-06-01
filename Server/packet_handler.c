#include "server_header.h"
#include "packet_header.h"

void packet_construct(int user_id, int io_byte)
{
    int rest_byte = io_byte;
    char *p = online_users[user_id].memberInfo.exOver->buffer;
    int packet_size = 0;

    if(online_users[user_id].memberInfo.prev_size != 0)    // 재조립을 기다리는 패킷 존재 유무
    {
        packet_size = online_users[user_id].memberInfo.packet_buf[0];  // 재조립 패킷의 크기
    }

    while(rest_byte > 0)    // IOCP 버퍼에 남은 데이터가 없어질때까지 
    {
        if(packet_size == 0)
        {
            packet_size = p[0];
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

int packet_handler(int id, char *packet_buffer)
{

    switch (packet_buffer[1])
    {
    case REGISTER:
        packet_register *packet = (packet_register* )packet_buffer;
        break;
    case LOGIN:
        packet_login *packet = (packet_login* )packet_buffer;
        break;
    case LOGOUT:
        packet_logout *packet = (packet_logout* )packet_buffer;
        break;
    case ENTER:
        packet_enter *packet = (packet_enter* )packet_buffer;
        break;
    case CHAT:
        packet_chat *packet = (packet_chat* )packet_buffer;
        break;
    case LEAVE:
        packet_quit *packet = (packet_quit* )packet_buffer;
        break;
    }
    return 0;
}

int packet_send(void *packet)
{
    int bytesTrans;
    
    return bytesTrans;
}