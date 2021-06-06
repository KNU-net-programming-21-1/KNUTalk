/*  채팅 내역과 방 목록을 클라이언트에 파일로 저장하는게 나은가?
    그러면 오프라인 상태일 때 생겨난 채팅 로그의 처리는?
*/


#define MAX_SIZE 100
#define ID_SIZE 21
#define PW_SIZE 21
#define MAX_ROOM_SIZE 100
#define BUF_SIZE 100
#define PACKET_SIZE 1024
#define READ	3
#define	WRITE	5

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <stdbool.h>
#include "cln_packet_header.h"
