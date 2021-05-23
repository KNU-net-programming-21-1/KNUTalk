#include "server_header.h"

/*  int id와 char* name을 입력으로 받아 room*에 등록
    최대 생성 가능한 방의 개수 체크
    room*에 메모리 할당 및 정보 등록 -> room_list에 추가
    return value    ret - 추가된 방을 포함한 현재 존재하는 방의 개수
                    LIMIT_REACHED - 최대 생성 가능한 방의 개수에 도달했을 경우
*/
int make_room(int id, char *name)                       // 방 생성 | room_list access (need mutex)
{
    int ret;
    room *new;

    if((ret = current_room_num()) == MAX_ROOM_SIZE)
    {
        return error_handling(LIMIT_REACHED);
    }
    else
    {
        new = (room*)malloc(sizeof(room));
        new->num_of_mem = 1;
        new->room_id = id;
        strcpy(new->room_name, name);

        // critical section
        
        // room_list에 추가 : list 구현 방법에 따라 변경 필요
        room_list[ret++] = *new;
        
        // end of critical section

        return ret;
    }   
}

/*  방의 id와 참가를 원하는 client의 정보(new_member)를 받아 해당 방의 존재 여부 확인 및 enter_member에 인자 전달
    enter_member function`s wrapper function
    return value    0 - 정상 종료
                    SEARCH_ERROR - id에 해당하는 방이 존재하지 않음
                    LIMIT_REACHED  - 해당 방에 참가 가능한 최대 인원 수 초과
*/
int enter_room(int id, member *new_member)              // 방 참가(enter_member wrapper function) | search_room(), enter_member() (need mutex)
{
    room *n;
    if((n = search_room(id)) == NULL)
    {
        return error_handling(SEARCH_ERROR);
    }
    else
    {
        return enter_member(n, new_member);
    }
}

/*  실제로 방에 인원을 추가
    return value    0 - 정상 종료
                    LIMIT_REACHED - 해당 방에 참가 가능한 최대 인원 수 초과
*/
int enter_member(room *target, member* new_member)            // 방에 인원 추가 | member_list access (need mutex)
{
    if (target->num_of_mem == MAX_SIZE)
    {
        return error_handling(LIMIT_REACHED);
    }
    else
    {
        new_member->cur_room = target->room_id;

        // critical section

        target->member_list[target->num_of_mem++] = *new_member;

        // end of critical section
        
        return 0;
    }
}

/*  현재 참가 중인 방에서 퇴장
    room_list에서 cur_room이 아닌 방도 퇴장 가능하게 하려면 수정 필요
    return value    0 - 정상 종료
*/
int quit_room(member *exit_user)                        // 방 나가기
{
    room *quit;
    quit = search_room(exit_user->cur_room);
    quit->num_of_mem--;

    /*
        room *quit->member_list에서 해당 유저 삭제 기능 추가 필요
    */

    return 0;
}

/*  존재하는 방 삭제
*/
int delete_room(int id)                                 // 방 삭제 | room_list access (need mutex)
{
    return 0;
}

/*  id를 받아 해당 방 구조체의 포인터를 반환
    current_room_num으로 현재 존재하는 방의 개수 찾아와서 탐색
    room_list의 구현 방법에 따라 탐색 알고리즘 수정 필요
    return value    room - id에 해당하는 방 구조체 포인터
                    NULL - 해당 방이 존재하지 않음
*/
room* search_room(int id)                               // room id로 방 검색 | room_list access (need mutex)
{
    int i, n;

    n = current_room_num();

    for (i = 0; i < n; i++)
    {
        if (room_list[i].room_id == id)
        {
            return &room_list[i];
        }
    }
    
    return NULL;
}

/*  현재 존재하는 방의 개수 탐색
    방 삭제 및 room_list의 구현에 따라 실제로 구현방법 달라져야 할 필요성
    return value    ret - 존재하는 방의 개수
*/
int current_room_num(void)                              // 현재 존재하는 방 개수 | room_list access (need mutex)
{
    int ret = 0;
    
    while(true)
    {
        if(room_list[ret].num_of_mem != 0)
        {
            ret++;
        }
        else
        {
            break;
        }
    }

    return ret;
}