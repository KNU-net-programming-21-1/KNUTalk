#include "server_header.h"

/*
    *******************
     TXT 파일 저장 내용
    *******************
    유저 ID,PW
    등록 순으로 int user_id부여 (from 0 ~ )
        -> 탐색의 편의성
    서버 부팅 시 파일에 등록되어있는 유저 리스트를 읽어와 registered_users 배열에 정보 저장
    등록되지 않은 유저는 registered_users[].member_info.user_id = -1 로 초기화
    채팅 내용은 클라이언트에 저장하는게 낫지 않을까...생각 중
    서버에 저장하면 일정기간 사용한 유저의 경우 로그인 시 불러와서 전송해야하는 데이터의 양이 지나치게 많아질 우려
    
*/




const char* f_name = "usr.txt";

int write_to_file(FILE *output, int type)           // DB 파일로 출력
{

}
int read_from_file(FILE *input, int type)           // DB 파일 읽어오기
{

}
int make_chat_log(FILE *output, room *target)           // 채팅 내용 파일에 저장
{

}

bool exist_user(char *id)                               // 유저 존재여부(for register)
{   
    int i;
}