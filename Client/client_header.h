/*  채팅 내역과 방 목록을 클라이언트에 파일로 저장하는게 나은가?
    그러면 오프라인 상태일 때 생겨난 채팅 로그의 처리는?
*/

/* fix debug error */
#pragma once                    

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

/* for debugging*/
#define DEBUG 0                                     // for debug , 0 -> normal

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <winSock2.h>
#include <process.h>
#include <windows.h>
#include <conio.h>

#define MAX_SIZE 100
#define ID_SIZE 21
#define PW_SIZE 21
#define MAX_ROOM_SIZE 100
#define BUF_SIZE 200                           
#define PACKET_SIZE 1024
#define READ	3
#define	WRITE	5

/* For UI */
#define LINE_SIZE 20
#define ROW 60
#define COL 65
#define TITLE   77
#define LOBBY   78
#define ROOM    79

/* Cursor move */
#define DELETE  8
#define SELECT  13
#define ESC     27
#define B_USER  61
#define REFRESH 63
#define UP      72
#define LEFT    75
#define RIGHT   77
#define DOWN    80

#define CLEAR system("cls")

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
    SOCKET hServSock;
} PER_HANDLE_DATA, * LPPER_HANDLE_DATA;

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
    LPPER_IO_DATA sendExOver;
    char packet_buf[PACKET_SIZE];  // packet constructor buffer
    int prev_size;  // 이전에 받은 패킷의 크기
} member_info;

typedef struct _member
{
    member_info memberInfo;
    int user_id;
    char id[ID_SIZE];  // 4 < strlen(id) < 20
    char pw[PW_SIZE];  // 10 < strlen(pw) < 20
    int cur_room;
    int blocked_user_num;
    int block_list[MAX_SIZE];
} member;

typedef struct _room
{
    char name[MAX_SIZE];
    int member_count;
}room;

static const char* ERROR_CODE[] = {      // ERROR_CODE = errno + OFFSET
    "buffer overflow",
    "search error",
    "function error",
    "I/O failure",
    "data duplicated",
    "IOCP error"
};

member user;
room room_info[MAX_SIZE];
char chat_buffer[LINE_SIZE][ID_SIZE + BUF_SIZE];
int menu_pointer;
int chat_pointer;

int user_main_thread(int port);
DWORD WINAPI WorkerThread(LPVOID CompletionPortIO);     // worker thread

void packet_construct(int io_byte);
int packet_send(SOCKET s, char *packet);

void init_console(void);
void set_console_size(int col, int row);
int title(void);
int lobby(void);
char* login(int select);
char* reg_session(int select);
char* chat_window(void);
int move_cursor(void);
void set_cursor_view(bool set);
void print_on_xy(int x, int y, char* buf);
void move_to_xy(int x, int y);

int error_handling(int err);
void crit_error_handling(char* err_message);