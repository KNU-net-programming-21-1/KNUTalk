#include "server_header.h"

/*
    *******************
     TXT 파일 저장 내용
    *******************
    유저 USER_ID, ID, PW
    등록 순으로 int user_id부여 (from 0 ~ )
        -> 탐색의 편의성
    서버 부팅 시 파일에 등록되어있는 유저 리스트를 읽어와 registered_users 배열에 정보 저장
    등록되지 않은 유저는 registered_users[].user_id = -1 로 초기화
    채팅 내용은 클라이언트에 저장하는게 낫지 않을까...생각 중
    서버에 저장하면 일정기간 사용한 유저의 경우 로그인 시 불러와서 전송해야하는 데이터의 양이 지나치게 많아질 우려
    
*/

/* 파일에 저장할 데이터 형식
    우선은 고정된 것만 넣었음
*/

typedef struct _file_data{
    int user_id;
    char id[ID_SIZE];
    char pw[PW_SIZE];
} fdata;

typedef struct _room_data{
    int room_id;
    char name[MAX_SIZE];
} rdata;



const char* user_file = "usr.txt";
const char* room_file = "room.txt";

void init_server(){
    int i;
    
    for(i=0;i<MAX_SIZE;i++){
		registered_users[i].user_id = -1;
        online_users[i].user_id = -1;
        room_list[i].num_of_mem = -1;
        online_users[i].memberInfo.exOver = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
        online_users[i].memberInfo.sendExOver = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
    }

    read_from_file();
}

void write_member_list_to_file()                                    // DB 파일로 출력
{
    int i;
    int new_users = registered_user();

    if(!(mem_list = fopen(user_file,"w+")))        // 파일 오픈 | 오류처리
    {
        fprintf(stderr,"FILE OPEN ERROR.\n");
        exit(1);
    }

    for (i = 0; i < new_users; i++)
    {
        fprintf(mem_list, "%d %s %s\n", registered_users[i].user_id, registered_users[i].id, registered_users[i].pw);
    }
    fclose(mem_list);
}

void write_room_list_to_file()                                    // DB 파일로 출력
{
    int i;
    int new_rooms = current_room_num();

    if(!(mem_list = fopen(room_file,"w+")))        // 파일 오픈 | 오류처리
    {
        fprintf(stderr,"FILE OPEN ERROR.\n");
        exit(1);
    }

    for (i = 1; i <= new_rooms; i++)
    {
        fprintf(mem_list, "%d %s\n", i, room_list[i].room_name);
    }
    fclose(mem_list);
}

void read_from_file()                               // DB 파일 읽어오기
{
    fdata tmp;
    rdata tmp_r;

    if(!(mem_list = fopen(user_file,"r+")))        // 파일 오픈 | 오류처리
    {
        fprintf(stderr,"FILE OPEN ERROR.\n");
        exit(1);
    }

#if DEBUG
    puts("Current Registered User List");
#endif
    
    while(fscanf(mem_list,"%d %s %s",&tmp.user_id, tmp.id, tmp.pw) != EOF)
    {
        registered_users[tmp.user_id].user_id = tmp.user_id;
        strcpy(registered_users[tmp.user_id].id,tmp.id);
        strcpy(registered_users[tmp.user_id].pw,tmp.pw);

#if DEBUG
        printf("USER ID : %d, ID: %s ,PW: %s\n", tmp.user_id, tmp.id, tmp.pw);
#endif
    }
    fclose(mem_list);
    
#if DEBUG
    puts("Current Registered Room List");
#endif
    
    if(!(mem_list = fopen(room_file,"r+")))        // 파일 오픈 | 오류처리
    {
        fprintf(stderr,"FILE OPEN ERROR.\n");
        exit(1);
    }
    
    while(fscanf(mem_list,"%d %s",&tmp_r.room_id, tmp_r.name) != EOF)
    {
        room_list[tmp_r.room_id].num_of_mem = 0;
        strcpy(room_list[tmp_r.room_id].room_name,tmp_r.name);

#if DEBUG
        printf("ROOM ID : %d, NAME: %s\n", tmp_r.room_id, tmp_r.name);
#endif
    }
}


