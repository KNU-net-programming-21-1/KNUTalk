#include "server_header.h"

/*  CS = client to server
    SC = server to client
*/

// PACKET TYPE DEFINE

#define LOGIN
#define ENTER
#define CHAT
#define LEAVE

struct CS_packet_login_
{
    char size;
    char type;
    char id[ID_SIZE];
    char pw[PW_SIZE];

}packet_login;  // C -> S 로그인 요청

struct SC_packet_login_accept
{
    char size;
    char type;
    bool accept;

}packet_accept; // S -> C 로그인 수락

struct CS_packet_logout_
{
    char size;
    char type;
    char id[ID_SIZE];

}packet_logout;  // C -> S 로그아웃 요청

struct SC_packet_logout_accept
{
    char size;
    char type;
    bool accept;

}packet_logout_accept; // S -> C 로그아웃 수락

struct CS_packet_enter
{
    char size;
    char type;
    int room_id;
    
}packet_enter;  // C -> S 방 입장 요청

struct CS_packet_leave
{
    char size;
    char type;
    int room_id;

}packet_quit;   // C -> S 방 퇴장 요청

struct SC_packet_enter
{
    char size;
    char type;
    int room_id;
    char user_name[ID_SIZE];
    
}packet_join;   // S -> C 방 구성원에게 user_name이 입장했다고 알림

struct SC_packet_leave
{
    char size;
    char type;
    int room_id;
    char user_name[ID_SIZE];

}packet_leave;  // S -> C 방 구성원에게 user_name이 퇴장했다고 알림

struct CS_packet_chat
{
    char size;
    char type;
    int room_id;
    char buf[BUF_SIZE];

}packet_chat;   // C -> S room_id의 방에 buf 내용 채팅 요청

struct SC_packet_chat
{
    char size;
    char type;
    int room_id;
    char buf[BUF_SIZE];

}packet_echo;   // S -> C room_id의 방에 buf 내용 echo