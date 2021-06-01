#include "server_header.h"

/*  int id와 char* name을 입력으로 받아 room_list에 등록
    최대 생성 가능한 방의 개수 체크
    return value    ret - 추가된 방을 포함한 현재 존재하는 방의 개수
                    LIMIT_REACHED - 최대 생성 가능한 방의 개수에 도달했을 경우
*/
int make_room(int id, char *name)                       // 방 생성 | room_list access (need mutex)
{
    int ret;
    int i;

    if((ret = current_room_num()) == MAX_ROOM_SIZE)
    {
        return error_handling(LIMIT_REACHED);
    }
    else
    {
        // critical section

        room_list[id].num_of_mem = 1;
        strcpy(room_list[id].room_name, name);
        for(i = 0; i < MAX_SIZE; i++)
        {
            room_list[id].member_list[i] = -1;
        }
        
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
    
    if(room_list[id].room_name == NULL)
    {
        return error_handling(SEARCH_ERROR);
    }
    else
    {
        return enter_member(id, new_member);
    }
}

/*  실제로 방에 인원을 추가
    return value    0 - 정상 종료
                    LIMIT_REACHED - 해당 방에 참가 가능한 최대 인원 수 초과
*/
int enter_member(int room_id, member* new_member)            // 방에 인원 추가 | member_list access (need mutex)
{
    // critical section
    
    int member_count = room_list[room_id].num_of_mem;
    
    // end of critical section

    if (member_count == MAX_SIZE)
    {
        return error_handling(LIMIT_REACHED);
    }
    else
    {
        new_member->cur_room = room_id;

        // critical section

        room_list[room_id].member_list[member_count++] = new_member->user_id;
        room_list[room_id].num_of_mem++;

        // end of critical section
        
        return 0;
    }
}

/*  현재 참가 중인 방에서 퇴장
    member.room_list에서 cur_room이 아닌 방도 퇴장 가능하게 하려면 수정 필요
    return value    0 - 정상 종료
*/
int quit_room(member *exit_user)                        // 방 나가기
{
    int i;

    for(i = 0; i < room_list[exit_user->cur_room].num_of_mem; i++)
    {
        if(room_list[exit_user->cur_room].member_list[i] == exit_user->user_id)
        {
            for(; i < room_list[exit_user->cur_room].num_of_mem; i++)
            {
                room_list[exit_user->cur_room].member_list[i] = room_list[exit_user->cur_room].member_list[i + 1];
            }
            break;
        }
    }

    room_list[exit_user->cur_room].num_of_mem--;

    return 0;
}

/*  존재하는 방 삭제
*/
int delete_room(int id)                                 // 방 삭제 | room_list access (need mutex)
{
    int i;

    // critical section

    strcpy(room_list[id].room_name, NULL);
    for(i = 0; i < room_list[id].num_of_mem; i++)
    {
        room_list[id].member_list[i] = -1;
    }
    room_list[id].num_of_mem = 0;
    
    // end of critical section

    return 0;
}

/*  현재 존재하는 방의 개수 탐색
    return value    ret - 존재하는 방의 개수
*/
int current_room_num(void)                              // 현재 존재하는 방 개수 | room_list access (need mutex)
{
    int ret;

    // critical section

    for(ret = 0; ret < MAX_SIZE; ret++)
    {
        if(room_list[ret].room_name == NULL)
        {
            continue;
        }
    }

    // end of critical section

    return ret;
}