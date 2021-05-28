  
/*
    ******************************
    구조체 변수 , 함수 정의 헤더파일
    ******************************
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <winSock2.h>

#define MAX_SIZE 100
#define ID_SIZE 21
#define PW_SIZE 21
#define MAX_ROOM_SIZE 100
#define BUF_SIZE 100

/* error codes */
#define OFFSET          10  // errno 개수에 따라 변경......
#define LIMIT_REACHED   -OFFSET
#define SEARCH_ERROR    -OFFSET + 1
#define FUNC_ERROR      -OFFSET + 2
#define DATA_FAILURE    -OFFSET + 3
#define DATA_DUPLICATE  -OFFSET + 4

typedef struct _member
{
    SOCKET s;
    char id[ID_SIZE];  // 4 < strlen(id) < 20
    char pw[PW_SIZE];  // 10 < strlen(pw) < 20
    bool is_online;
    int room_list[MAX_ROOM_SIZE];
    int cur_room;
    char block_list[MAX_SIZE];
    char msg_buf[BUF_SIZE];
} member;

typedef struct _room    // 방의 id도 구조체 내부에 넣을거면 굳이 room_list를 배열로 선언할 이유 없어보임
{
    int room_id;
    char room_name[MAX_SIZE];
    int num_of_mem;
    member member_list[MAX_SIZE];
} room;

/* 서버가 유지할 데이터 */

//  DATA RACE BLOCK
room room_list[MAX_ROOM_SIZE];      // 방 목록(linked list로 구현?)
member online_users[MAX_SIZE];      // 접속중인 유저 리스트
member registerd_users[MAX_SIZE];   // 등록된 모든 유저 리스트
//  DATA RACE BLOCK

FILE* mem_list;                     // 유저 목록이 저장될 파일(서버 종료 시 저장 | 일정 시간 경과 후 주기적으로 백업)


/*
    #define OFFSET          10  // errno 개수에 따라 변경......
    #define LIMIT_REACHED   -OFFSET
    #define SEARCH_ERROR    -OFFSET + 1
    #define FUNC_ERROR      -OFFSET + 2
    #define DATA_FAILURE    -OFFSET + 3
    #define DATA_DUPLICATE  -OFFSET + 4
*/
static const char *ERROR_CODE[] = {      // ERROR_CODE = errno + OFFSET
    "buffer overflow",
    "search error",
    "function error",
    "I/O failure",
    "data duplicated"
};

/* 데이터 베이스 관리 함수 | DB_management.c */

int write_to_file(FILE *output, int type);               // DB 파일로 출력
int read_from_file(FILE *input, int type);               // DB 파일 읽어오기
int make_chat_log(FILE *output, room *target);           // 채팅 내용 파일에 저장
bool exist_user(char *id);                              // 유저 검색(계정 등록 위해 존재 유무 파악)

/* 방에 관련된 함수 | room_management.c */

int make_room(int id, char *name);                  // 방 생성
int enter_room(int id, member *new_member);         // 방 참가(enter_member wrapper function)
int enter_member(room *target, member *new_member); // 방에 인원 추가
int quit_room(member *exit_user);                   // 방 나가기
int delete_room(int id);                            // 방 삭제
room* search_room(int id);                          // room id로 방 검색
int current_room_num(void);                         // 현재 존재하는 방 개수

/* 채팅 기능 함수 | chat_processor.c */

int add_block_list(char *user_name);              // 사용자 차단하기
int send_message(int room_number);                // 메시지 입력

/* 서버에서 처리할 함수 | user_interaction.c */

int login(char *ID, char *PW);                  // 서버에 로그인
int member_register(char *ID, char *PW);         // 서버에 계정 등록

/* Thread | thread.c */

int accept_thread(int port);                            // client -> server accept thread
DWORD WINAPI WorkerThread(LPVOID CompletionPortIO);     // worker thread

/* Error Handling 함수 | error_handling.c */

int error_handling(int errno);                          // print stderr | return errno
void crit_error_handling(char* err_message);            // print stderr | exit(-1)