#include "server_header.h"
/*
    추가해야 할 패킷
        - ...
*/

/*  
    CS = client to server
    SC = server to client
*/

// PACKET TYPE DEFINE
#define QUIT		0		// 여기에 넣어두는 편이 보기 수월할거라 생각해서 추가해두겠습니다.
#define REGISTER    1
#define LOGIN       2
#define LOGOUT      3
#define ENTER       4
#define LEAVE       5
#define CHAT        6
#define BLOCK       7
#define ROOMINFO    8
#define MAKEROOM    9

// PACKET TYPE DEFINE END

// PACKET TYPE REGISTER
typedef struct CS_packet_register
{
    short size;
    char type;
    char id[ID_SIZE];
    char pw[PW_SIZE];

}packet_register;   // C -> S 계정 등록 요청

/*  accept  true - 등록 성공
            DATA_DUPLICATE - 이미 존재하는 아이디
            DATA_FAILURE - 잘못된 입력양식
*/
typedef struct SC_packet_registered
{
    short size;
    char type;
    int accept;

}packet_registered; // S -> C 계정 등록 완료 여부

// PACKET TYPE LOGIN
typedef struct CS_packet_login_
{
    short size;
    char type;
    char id[ID_SIZE];
    char pw[PW_SIZE];

}packet_login;  // C -> S 로그인 요청

typedef struct SC_packet_login_accept
{
    short size;
    char type;
    bool accept;
    int user_id;    // 서버에서 할당한 online_users[] index 클라이언트에게 전달

}packet_accept; // S -> C 로그인 수락

// PACKET TYPE LOGOUT
typedef struct CS_packet_logout_
{
    short size;
    char type;
    int id;

}packet_logout;  // C -> S 로그아웃 요청

typedef struct SC_packet_logout_accept
{
    short size;
    char type;
    bool accept;

}packet_logout_accept; // S -> C 로그아웃 수락

// PACKET TYPE ENTER
typedef struct CS_packet_enter
{
    short size;
    char type;
    int room_id;
    
}packet_enter;  // C -> S 방 입장 요청

typedef struct SC_packet_enter
{
    short size;
    char type;
    bool accept;    // 정원 초과일 경우 요청한 클라이언트에게만 실패했다고 전송
    int room_id;
    char user_name[ID_SIZE];
    
}packet_join;   // S -> C 방 구성원에게 user_name이 입장했다고 알림

// PACKET TYPE LEAVE
typedef struct CS_packet_leave
{
    short size;
    char type;
    int room_id;

}packet_quit;   // C -> S 방 퇴장 요청

typedef struct SC_packet_leave
{
    short size;
    char type;
    int room_id;
    char user_name[ID_SIZE];

}packet_leave;  // S -> C 방 구성원에게 user_name이 퇴장했다고 알림

// PACKET TYPE CHAT
typedef struct CS_packet_chat
{
    short size;
    char type;
    int room_id;
    char buf[BUF_SIZE];

}packet_chat;   // C -> S room_id의 방에 buf 내용 채팅 요청

typedef struct SC_packet_chat
{
    short size;
    char type;
    char user_name[ID_SIZE];    // 채팅 발신자
    char buf[BUF_SIZE];

}packet_echo;   // S -> C room_id의 방에 buf 내용 echo

// PACKET TYPE BLOCK

typedef struct CS_packet_block
{
    short size;
    char type;
    char user_name[ID_SIZE];

}packet_block;  // C -> S name 사용자 블락

typedef struct SC_packet_blocked
{
    short size;
    char type;
    bool accept;    // 존재하는 아이디가 아닐 경우 false

}packet_blocked;    // S -> C 사용자 블락 완료 여부

// PACKET TYPE ROOMINFO

typedef struct CS_packet_request
{
    short size;
    char type;

}packet_request;    // C -> S 로그인 세션 완료 후 방 정보 요청

typedef struct SC_packet_roominfo
{
    short size;
    char type;
    int room_member[MAX_SIZE];  // 방에 있는 사람의 수 || index -> room_member_num
    // 방 이름까지 보내기엔 구조체가 너무 커짐 -> 대안...? 자신이 들어가 있는 방의 이름과 id를 클라이언트에 저장

}packet_roominfo;

// PACKET TYPE MAKEROOM

typedef struct CS_packet_makeroom
{
    short size;
    char type;
    int user_id;
    char room_name[MAX_SIZE];

}packet_makeroom;       // C -> S 방 생성 요청

typedef struct SC_packet_complete	// 위에 구조체랑 재정의 오류 나서 임의로 이름 바꿨습니다!
{
    short size;
    char type;
    bool accept;
    int room_id;

}packet_complete;       // S -> C 방 생성 완료 여부 및 room_id