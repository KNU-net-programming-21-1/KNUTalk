#include "client_header.h"
#include "cln_packet_header.h"
#include <conio.h>

#define ROW 60
#define COL 65
#define CURSOR_SIZE 1


void init_console(void)
{
    char buf[MAX_SIZE];
    sprintf(buf, "mode con cols=%d lines=%d", COL, ROW);
    system(buf);
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

void lobby(void)
{

}

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

    scanf_s("%s", 20, packet->id);
    printf("\n사용할 비밀번호를 입력해주세요 || ");
    scanf_s("%s", 20, packet->pw);
    printf("\n서버와 통신 중 입니다...\n");
    
    return (char*)packet;
}

void chat_window(void)
{

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
                    default:
                        return c;
                }
            }
            else if(c == 13)
            {
                return SELECT;
            }
            else if(c == 27)
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