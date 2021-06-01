#include "server_header.h"
/*
    추가해야 할 패킷
        - 입장 가능한 방의 리스트 정보를 담은 패킷(S -> C)
        - 생성한 방의 이름과 유저 정보를 담은 패킷(C -> S)
        - ...
*/

/*  
    CS = client to server
    SC = server to client
*/

// PACKET TYPE DEFINE

#define REGISTER    1
#define LOGIN       2
#define LOGOUT      3
#define ENTER       4
#define LEAVE       5
#define CHAT        6

// PACKET TYPE DEFINE END

// PACKET REGISTER
typedef struct CS_packet_register
{
    char size;
    char type;
    char id[ID_SIZE];
    char pw[PW_SIZE];

}packet_register;   // C -> S 계정 등록 요청

typedef struct SC_packet_registered
{
    char size;
    char type;
    bool accept;
}packet_registered; // S -> C 계정 등록 완료 여부

// PACKET LOGIN
typedef struct CS_packet_login_
{
    char size;
    char type;
    char id[ID_SIZE];
    char pw[PW_SIZE];

}packet_login;  // C -> S 로그인 요청

typedef struct SC_packet_login_accept
{
    char size;
    char type;
    bool accept;

}packet_accept; // S -> C 로그인 수락

// PACKET LOGOUT
typedef struct CS_packet_logout_
{
    char size;
    char type;
    char id[ID_SIZE];

}packet_logout;  // C -> S 로그아웃 요청

typedef struct SC_packet_logout_accept
{
    char size;
    char type;
    bool accept;

}packet_logout_accept; // S -> C 로그아웃 수락

// PACKET ENTER
typedef struct CS_packet_enter
{
    char size;
    char type;
    int room_id;
    
}packet_enter;  // C -> S 방 입장 요청

typedef struct SC_packet_enter
{
    char size;
    char type;
    int room_id;
    char user_name[ID_SIZE];
    
}packet_join;   // S -> C 방 구성원에게 user_name이 입장했다고 알림

// PACKET LEAVE
typedef struct CS_packet_leave
{
    char size;
    char type;
    int room_id;

}packet_quit;   // C -> S 방 퇴장 요청

typedef struct SC_packet_leave
{
    char size;
    char type;
    int room_id;
    char user_name[ID_SIZE];

}packet_leave;  // S -> C 방 구성원에게 user_name이 퇴장했다고 알림

// PACKET CHAT
typedef struct CS_packet_chat
{
    char size;
    char type;
    int room_id;
    char buf[BUF_SIZE];

}packet_chat;   // C -> S room_id의 방에 buf 내용 채팅 요청

typedef struct SC_packet_chat
{
    char size;
    char type;
    int room_id;
    char buf[BUF_SIZE];

}packet_echo;   // S -> C room_id의 방에 buf 내용 echo