#include "client_header.h"
#include "cln_packet_header.h"

#define CURSOR_SIZE 1


void init_console(void)
{
    char buf[MAX_SIZE];
    sprintf(buf, "mode con cols=%d lines=%d", COL, ROW);
    system(buf);
    system("title KNUTALK");
    set_cursor_view(FALSE);
    CLEAR;
}

void set_console_size(int col, int row)
{
    char buf[MAX_SIZE];
    sprintf(buf, "mode con cols=%d lines=%d", col, row);
    system(buf);
    CLEAR;
}

int title(void)
{
    int cursor = FALSE;
    int select = 30;
    move_to_xy(0, 3);
    printf("            ●　　　●　　●　　　●　　●　　　　●　\n");
    printf("            ●　　●　　　●●　　●　　●　　　　●　\n");
    printf("            ●　●　　　　●●　　●　　●　　　　●　\n");
    printf("            ●●　　　　　●　●　●　　●　　　　●　\n");
    printf("            ●　●　　　　●　●　●　　●　　　　●　\n");
    printf("            ●　　●　　　●　　●●　　●　　　　●　\n");
    printf("            ●　　　●　　●　　●●　　●　　　　●　\n");
    printf("            ●　　　　●　●　　　●　　　●●●●　\n\n");
    printf("          ●●●●●●●　　　　　　　●　　●　　　　\n");
    printf("          　    ●　　　　　　　　　　●　　●　　　　\n");
    printf("          　    ●　　　　●●●　　　●　　●　　●　\n");
    printf("          　    ●　　　●　　　●　　●　　●　●　　\n");
    printf("          　    ●　　　　●●●●　　●　　●●　　　\n");
    printf("          　    ●　　　●　　　●　　●　　●●　　　\n");
    printf("          　    ●　　　●　　　●　　●　　●　●　　\n");
    printf("          　    ●　　　　●●●　●　●　　●　　●　\n");
    
    print_on_xy(21, select, "->");
    print_on_xy(24, 30, "   계정 등록");
    print_on_xy(24, 32, "    로그인");
    print_on_xy(24, 34, "     종료");

    for( ; cursor != SELECT && cursor != ESC; )
    {
        cursor = move_cursor();
        switch (cursor)
        {
        case UP:
            if (select > 30)
            {
                print_on_xy(21, select, "   ");
                select = select - 2; 
                print_on_xy(21, select, "->");
            }
            break;
        case DOWN:
            if (select < 34)
            {
                print_on_xy(21, select, "   ");
                select = select + 2;
                print_on_xy(21, select, "->");
            }
            break;
        case SELECT:
            break;
        case ESC:
            select = 34;
            break;
        }
    }

    switch (select)
    {
    case 30:
        return REGISTER;
    case 32:
        return LOGIN;
    case 34:
        return QUIT;
    }
}

int lobby(void)
{
    int cursor = FALSE;
    int room_select = 1;
    int room_count = 0;
    int i;

    printf("ID / NAME                                접속자/최대인원\n");
    for(i = 1; i < MAX_SIZE; i++)
    {
        if(room_info[i].name[0] == NULL)
        {
            break;
        }
        printf("%3d%20s                  %6d/%6d\n", i, room_info[i].name, room_info[i].member_count, MAX_SIZE);
        room_count++;
    }

    print_on_xy(0, 56, "방에 참가하기(Enter)");
    print_on_xy(0, 58, "새로운 방 생성(M/m)");
    print_on_xy(50, 56, "새로고침(F5)");
    print_on_xy(50, 58, "로그아웃(ESC)");
    print_on_xy(58, room_select, "<-");
    for (; cursor != SELECT; )
    {
        cursor = move_cursor();
        switch (cursor)
        {
        case UP:
            if (room_select > 1)
            {
                print_on_xy(58, room_select, "  ");
                room_select = room_select - 1;
                print_on_xy(58, room_select, "<-");
            }
            break;
        case DOWN:
            if (room_select < room_count)
            {
                print_on_xy(58, room_select, "  ");
                room_select = room_select + 1;
                print_on_xy(58, room_select, "<-");
            }
            break;
        case REFRESH + MAX_SIZE:
            return REFRESH + MAX_SIZE;
        case SELECT:
            if(room_count != 0)
            {
                return room_select;
            }
            cursor = FALSE;
            break;
        case ESC:
            return QUIT;
        case 'M':
            return MAKEROOM + MAX_SIZE;
        case 'm':
            return MAKEROOM + MAX_SIZE;
        }
    }
    return room_select;
}

/*printf("\n*********************\n\n");
printf("LOGOUT < 3 >\t\t");
printf("ENTER < 4 >\t");
printf("MAKEROOM < 9 >\t");
printf("\n*********************\n\n");
printf("menu : ");*/

char* login(int select)
{
    packet_login* packet = (packet_login*)malloc(sizeof(packet_login));
    int i;
    packet->type = LOGIN;
    packet->size = sizeof(packet_login);

    set_console_size(100, COL / 2);
    printf("●　　　　　　　　　　　　　　　　●　　　　　　　\n");
    printf("●　　　　　　　　　　　　　　　　　　　　　　　　\n");
    printf("●　　　●●●　　　　●●●●　　●　　●●●●　\n");
    printf("●　　●　　　●　　●　　　●　　●　　●　　　●\n");
    printf("●　　●　　　●　　●　　　●　　●　　●　　　●\n");
    printf("●　　●　　　●　　●　　　●　　●　　●　　　●\n");
    printf("●　　●　　　●　　　●●●●　　●　　●　　　●\n");
    printf("●　　　●●●　　　　　　　●　　●　　●　　　●\n");
    printf("                    ●　　　●　　　　　　　　　　\n");
    printf("                      ●●●　　　　　　　　　　　\n\n");
    printf("esc 키를 입력해 타이틀로 돌아가기\n\n");

    switch (select)
    {
    case LOGIN:
        printf("\n");
        break;    
    case FALSE:
        printf("아이디 또는 비밀번호가 일치하지 않습니다.\n");
        break;
    }
    
    

    printf("\nID를 입력해주세요 || ");

    packet->id[0] = _getch();
    if ( packet->id[0] == 27)
    {
        free(packet);
        return NULL;
    }
    else
    {
        printf("%c", packet->id[0]);
        packet->id[1] = '\0';
    }

    scanf("%s", &packet->id[1]);
    printf("\n비밀번호를 입력해주세요 || ");
    scanf("%s", packet->pw);
    printf("\n서버와 통신 중 입니다...\n");
    
    return (char*)packet;
}

char* reg_session(int select)
{
    packet_register* packet;
    packet = (packet_register*)malloc(sizeof(packet_register));
    packet->type = REGISTER;
    packet->size = sizeof(packet_register);

    set_console_size(100, COL / 2);
    printf("                                    ●　　　　　　　　　●　　　　　　　　　　　　\n");
    printf("                                                        ●　　　　　　　　　　　　\n");
    printf("●●●　　●●●　　　　●●●●　　●　　　●●●　　●●●　　●●●　　　●●●\n");
    printf("●　　　●　　　●　　●　　　●　　●　　●　　　●　　●　　●　　　●　　●　　\n");
    printf("●　　　●●●●●　　●　　　●　　●　　　●●　　　　●　　●●●●●　　●　　\n");
    printf("●　　　●　　　　　　●　　　●　　●　　　　　●　　　●　　●　　　　　　●　　\n");
    printf("●　　　●　　　●　　　●●●●　　●　　●　　　●　　●　　●　　　●　　●　　\n");
    printf("●　　　　●●●　　　　　　　●　　●　　　●●●　　　●●　　●●●　　　●　　\n");
    printf("                      ●　　　●　　　　　　　　　　　　　　　　　　　　　　　　　\n");
    printf("                        ●●●　　　　　　　　　　　　　　　　　　　　　　　　　　\n\n");
    printf("esc 키를 입력해 타이틀로 돌아가기\n\n");
    
    switch (select)
    {
    case REGISTER:
        printf("\n");
        break;    
    case DATA_DUPLICATE:
        printf("이미 존재하는 ID입니다. 다시 시도해주세요.\n");
        break;
    case DATA_FAILURE:
        printf("ID는 4~20자리, 비밀번호는 10~20자리 범위 내에서 입력해주세요.\n");
        break;
    }
    
    printf("\n사용할 ID를 입력해주세요 || ");

    packet->id[0] = _getch();
    if ( packet->id[0] == ESC)
    {
        free(packet);
        return NULL;
    }
    else
    {
        printf("%c", packet->id[0]);
        packet->id[1] = '\0';
    }

    scanf("%s", &packet->id[1]);
    printf("\n사용할 비밀번호를 입력해주세요 || ");
    scanf("%s", packet->pw);
    printf("\n서버와 통신 중 입니다...\n");
    
    return (char*)packet;
}

char* chat_window(void)
{
    int cursor = FALSE;
    int i;
    char buf[BUF_SIZE];
    packet_chat* packet = (packet_chat*)malloc(sizeof(packet_chat));
    packet->room_id = user.cur_room;
    packet->size = sizeof(packet_chat);
    packet->type = CHAT;

    set_cursor_view(TRUE);
    print_on_xy(50, 0, room_info[user.cur_room].name);
    print_on_xy(5, 62, "사용자 블락하기(F3)");
    print_on_xy(75, 62, "로비로 돌아가기(ESC)");
    print_on_xy(1, 56, "->                                                                                           ");
    move_to_xy(4, 56);
    for(i = 0; i < BUF_SIZE; )
    {
        cursor = move_cursor();
        switch (cursor)
        {
        case SELECT:
            if(i == 0)
            {
                cursor = FALSE;
            }
            else
            {
                if(i == BUF_SIZE)
                {
                    buf[i - 1] = '\0';
                }
                else
                {
                    buf[i] = '\0';
                }
                strcpy(packet->buf, buf);
                
                move_to_xy(4, 56);
                return (char*)packet;
            }
            break;

        case B_USER + MAX_SIZE:
            menu_pointer = BLOCK;
            return NULL;

        case ESC:
            return NULL;

        case DELETE:
            if (i >= 0)
            {
                if (i != 0)
                {
                    i--;
                }
                print_on_xy(4 + i, 56, " ");
                move_to_xy(4 + i, 56);
                buf[i] = '\0';
            }
            break;

        default:
            putc(cursor, stdout);
            buf[i] = cursor;
            i++;
            break;
        }
    }
    return (char*)packet;
}

int move_cursor(void)
{
    char c;
    for(;;)
    {
        if(_kbhit())
        {
            c = _getch();
            if(c == 0xE0 || c == 0x00)
            {
                c = _getch();
                switch (c)
                {
                    case UP:
                        return UP;
                    case DOWN:
                        return DOWN;
                    case LEFT:
                        return LEFT;
                    case RIGHT:
                        return RIGHT;
                    case REFRESH:
                        return REFRESH + MAX_SIZE;
                    case B_USER:
                        return B_USER + MAX_SIZE;
                    default:
                        return c;
                }
            }
            else if(c == DELETE)
            {
                return DELETE;
            }
            else if(c == SELECT)
            {
                return SELECT;
            }
            else if(c == ESC)
            {
                return ESC;
            }
            else
            {
                return c;
            }
        }
    }
}

void set_cursor_view(bool set)
{
    CONSOLE_CURSOR_INFO cursor_info = {0, };
    cursor_info.dwSize = CURSOR_SIZE;
    cursor_info.bVisible = set;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}

void print_on_xy(int x, int y, char* buf)
{
    COORD pos = {x, y};
    CONSOLE_SCREEN_BUFFER_INFO prev_pos;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &prev_pos);
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    printf("%s", buf);
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), prev_pos.dwCursorPosition);
}

void move_to_xy(int x, int y)
{
    COORD pos = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}