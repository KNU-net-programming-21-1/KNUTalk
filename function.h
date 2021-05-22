/*
    ******************************

    구조체 변수 , 함수 정의 헤더파일

    ******************************
*/

#define MAX_SIZE 100
#define MAX_ROOM_SIZE 100
#define BUF_SIZE 100

#include <stdbool.h>
#include <stdio.h>

typedef struct _room
{
    int room_id;
    char room_name[MAX_SIZE];
    int num_of_mem;
} room;

typedef struct _member
{
    char id[MAX_SIZE];
    char pw[MAX_SIZE];
    int is_online;
    int room_list[MAX_ROOM_SIZE];
    int cur_room;
    char block_list[MAX_SIZE];
    char msg_buf[BUF_SIZE];
} member;

/* 서버가 유지할 데이터 */

room room_list[MAX_ROOM_SIZE];      // 방 목록
FILE* mem_list;                     // 유저 목록이 저장될 파일
member online_users[MAX_SIZE];      // 접속중인 유저 리스트

/* 방에 관련된 함수 */

void make_room();                   // 방 생성
void enter_room();                  // 방 참가
void quit_room();                   // 방 나가기

/* 채팅 기능 함수 */

void add_block_list();              // 사용자 차단하기
void send_message();                // 메시지 입력

/* 서버에서 처리할 함수 */

bool login();                       // 서버에 로그인
member add_member();                // 서버 가입
void make_chat_log();               // 채팅 내용 파일에 저장


