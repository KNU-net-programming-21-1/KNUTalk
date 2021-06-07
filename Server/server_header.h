/*
    ******************************
    구조체 변수 , 함수 정의 헤더파일
    ******************************
*/

/* fix debug error */
#pragma once                    

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

/* for debugging*/
#define DEBUG 1                                     // for debug , 0 -> normal

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <winSock2.h>

#define MAX_SIZE 100
#define ID_SIZE 21
#define PW_SIZE 21
#define MAX_ROOM_SIZE 100
#define BUF_SIZE 200                                // MAKEROOM 패킷 보내면 100 바이트 밖에 못 받아서 임시조치로 200으로 늘려놨습니다.
#define PACKET_SIZE 1024
#define READ	3
#define	WRITE	5

/* error codes */
#define OFFSET          10  // errno 개수에 따라 변경......
#define LIMIT_REACHED   -OFFSET
#define SEARCH_ERROR    -OFFSET + 1
#define FUNC_ERROR      -OFFSET + 2
#define DATA_FAILURE    -OFFSET + 3
#define DATA_DUPLICATE  -OFFSET + 4
#define IOCP_ERROR      -OFFSET + 5

typedef struct    // socket info
{
    SOCKET hClntSock;
    SOCKADDR_IN clntAdr;
    int user_index;
} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

typedef struct    // buffer info
{
    OVERLAPPED overlapped;
    WSABUF wsaBuf;
    char buffer[BUF_SIZE];
    int rwMode;    // READ or WRITE
} PER_IO_DATA, *LPPER_IO_DATA;

typedef struct _member_info // 패킷 처리를 위한 구조체
{
    SOCKET s;
    LPPER_IO_DATA exOver;   // IOCP buffer(overlapped 구조체 확장)
    char packet_buf[PACKET_SIZE];  // packet constructor buffer
    int prev_size;  // 이전에 받은 패킷의 크기
} member_info;

typedef struct _member
{
    member_info memberInfo;
    int user_id;
    char id[ID_SIZE];  // 4 < strlen(id) < 20
    char pw[PW_SIZE];  // 10 < strlen(pw) < 20
    bool is_online;
    int room_list[MAX_ROOM_SIZE];
    int cur_room;
    int blocked_user_num;
    int block_list[MAX_SIZE];
} member;

typedef struct _room
{
    char room_name[MAX_SIZE];
    int num_of_mem;
    int member_list[MAX_SIZE];
} room;

/* 서버가 유지할 데이터 */

//  DATA RACE BLOCK
room room_list[MAX_ROOM_SIZE];      // 방 목록
member online_users[MAX_SIZE];      // 접속중인 유저 리스트 -> 서버에 접속한 순서대로 인덱스에 채워넣음 || 로그아웃 인원 생기면 그 자리 비우고 새 유저에게 그 공간 할당
member registered_users[MAX_SIZE];  // 등록된 모든 유저 리스트 -> 등록한 순서대로 인덱스와 같은 id 부여 || client와 연결 시 online_users 배열에 정보 입력하기 위한 저장공간
//  DATA RACE BLOCK

FILE* mem_list;                     // 유저 목록이 저장될 파일(서버 종료 시 저장 | 일정 시간 경과 후 주기적으로 백업)

/*
    OFFSET          10  // errno 개수에 따라 변경......
    LIMIT_REACHED   -OFFSET
    SEARCH_ERROR    -OFFSET + 1
    FUNC_ERROR      -OFFSET + 2
    DATA_FAILURE    -OFFSET + 3
    DATA_DUPLICATE  -OFFSET + 4
    IOCP_ERROR      -OFFSET + 5
*/
static const char *ERROR_CODE[] = {      // ERROR_CODE = errno + OFFSET
    "buffer overflow",
    "search error",
    "function error",
    "I/O failure",
    "data duplicated",
    "IOCP error"
};

/* 데이터 베이스 관리 함수 | DB_management.c */
void init_server();                                      // 서버 실행시 데이터 초기화 및 동기화 (임시로 DB에 뒀습니다)
void write_to_file();                                    // DB 파일로 출력
void read_from_file();                                   // DB 파일 읽어오기 | 서버 가동시 최초 실행되어 registered_users 가져옴
int make_chat_log(FILE *output, room *target);           // 채팅 내용 파일에 저장

/* 방에 관련된 함수 | room_management.c */

int make_room(int id, char *name);                  // 방 생성
int enter_room(int room_id, int user_id);           // 방 참가
int quit_room(int user_id);                         // 방 나가기
int delete_room(int room_id);                            // 방 삭제
int current_room_num(void);                         // 현재 존재하는 방 개수
int find_empty_room(void);                          // 방 생성이 가능한 room_id

/* 채팅 기능 함수 | chat_processor.c */

int add_block_list(int user_id, char *user_name);                   // 사용자 차단하기
int echo_message(int user_id, int room_number, char *message);      // 방 인원에게 user_id의 채팅내용 전송

/* 서버에서 처리할 함수 | user_interaction.c */

int login(int user_id, char *ID, char *PW);     // 서버에 로그인
int logout(int user_id);                        // 서버에서 로그아웃
int member_register(int user_id, char *ID, char *PW);        // 서버에 계정 등록
int search_user(char *name);                    // char name으로 int user_id 검색
int registered_user();                          // 가입된 유저 수 반환
/* 패킷 처리 함수 | packet_handler.c */

void packet_construct(int user_id, int io_byte);    // IOCP 버퍼 내의 패킷 조립
int packet_handler(int user_id, char *packet_buffer);    // 패킷의 타입에 따른 처리
int packet_send(int user_id, char *packet);         // 패킷 전송

/* Thread | thread.c */

// registered_users, online_users 등 전역변수에 대한 참조가 너무 많음...
// 전부 mutex를 이용하면 성능 저하 우려 -> 메세지 큐 이용해서 commit하는 방법도 있지만 일단 보류
int accept_thread(int port);                            // client -> server accept thread
DWORD WINAPI WorkerThread(LPVOID CompletionPortIO);     // worker thread

/* Error Handling 함수 | error_handling.c */

int error_handling(int err);                          // print stderr | return errno
void crit_error_handling(char* err_message);            // print stderr | exit(-1)