/*  채팅 내역과 방 목록을 클라이언트에 파일로 저장하는게 나은가?
    그러면 오프라인 상태일 때 생겨난 채팅 로그의 처리는?
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

typedef struct    // server socket info
{
    SOCKET hServSock;
    SOCKADDR_IN ServAdr;
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
    char packet_buf[PACKET_SIZE];  // packet constructor buffer
    int prev_size;  // 이전에 받은 패킷의 크기
} member_info;

typedef struct _member
{
    member_info memberInfo;
    int user_id;
    char id[ID_SIZE];  // 4 < strlen(id) < 20
    char pw[PW_SIZE];  // 10 < strlen(pw) < 20
    int room_list[MAX_ROOM_SIZE];
    int cur_room;
    int blocked_user_num;
    int block_list[MAX_SIZE];
} member;

int user_main_thread(port);
DWORD WINAPI WorkerThread(LPVOID CompletionPortIO);     // worker thread

static const char* ERROR_CODE[] = {      // ERROR_CODE = errno + OFFSET
    "buffer overflow",
    "search error",
    "function error",
    "I/O failure",
    "data duplicated",
    "IOCP error"
};