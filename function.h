/*
    ******************************

    ����ü ���� , �Լ� ���� �������

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

/* ������ ������ ������ */

room room_list[MAX_ROOM_SIZE];      // �� ���
FILE* mem_list;                     // ���� ����� ����� ����
member online_users[MAX_SIZE];      // �������� ���� ����Ʈ

/* �濡 ���õ� �Լ� */

void make_room();                   // �� ����
void enter_room();                  // �� ����
void quit_room();                   // �� ������

/* ä�� ��� �Լ� */

void add_block_list();              // ����� �����ϱ�
void send_message();                // �޽��� �Է�

/* �������� ó���� �Լ� */

bool login();                       // ������ �α���
member add_member();                // ���� ����
void make_chat_log();               // ä�� ���� ���Ͽ� ����


