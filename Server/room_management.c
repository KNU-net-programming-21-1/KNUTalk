#include "server_header.h"
#include "packet_header.h"

/*  int user_id(방 생성 요청한 유저)와 char* name을 입력으로 받아 room_list에 등록
    최대 생성 가능한 방의 개수 체크
    return value    id - 새로 생성된 방의 id
                    LIMIT_REACHED - 최대 생성 가능한 방의 개수에 도달했을 경우
*/
int make_room(int user_id, char *name)                       // 방 생성 | room_list access (need mutex)
{
    int id = find_empty_room();
    int i;
    packet_complete packet;
    
    packet.size = sizeof(packet_complete);
    packet.type = MAKEROOM;

    if(id == MAX_ROOM_SIZE)
    {
        packet.accept = false;
        packet_send(user_id, &packet);
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

        room_list[id].member_list[0] = online_users[user_id].user_id;
        
        // end of critical section

        packet.accept = true;
        packet.room_id = id;
        packet_send(user_id, &packet);

        return id;
    }   
}

/*  방의 id와 참가를 원하는 client의 id를 받아 방에 인원 참가 & 구성원에게 알림
    return value    0 - 정상 종료
                    SEARCH_ERROR - id에 해당하는 방이 존재하지 않음
                    LIMIT_REACHED  - 해당 방에 참가 가능한 최대 인원 수 초과
*/
int enter_room(int room_id, int user_id)              // 방 참가 (need mutex)
{
    int member_count;
    int i;
    packet_join packet;

    packet.size = sizeof(packet_join);
    packet.type = ENTER;
    packet.room_id = room_id;

    // critical section
    
    member_count = room_list[room_id].num_of_mem;
    
    // end of critical section

    if (member_count == -1)
    {
        packet.accept = false;
        packet_send(user_id, &packet);

        return error_handling(SEARCH_ERROR);
    }
    else if(member_count == MAX_SIZE)
    {
        packet.accept = false;
        packet_send(user_id, &packet);

        return error_handling(LIMIT_REACHED);
    }
    else
    {
        // critical section

        room_list[room_id].member_list[member_count] = online_users[user_id].user_id;
        room_list[room_id].num_of_mem++;        
        strcpy(packet.user_name, online_users[user_id].id);
        packet.accept = true;
        packet.room_id = room_id;

        for(i = 0; i < room_list[room_id].num_of_mem; i++)
        {
            packet_send(room_list[room_id].member_list[i], &packet);
        }

        // end of critical section

        return 0;
    }
}

/*  현재 참가 중인 방에서 퇴장
    member.room_list에서 cur_room이 아닌 방도 퇴장 가능하게 하려면 수정 필요
    return value    0 - 정상 종료
*/
int quit_room(int user_id)                        // 방 나가기
{
    int i;
    int current = online_users[user_id].cur_room;

    for(i = 0; i < room_list[current].num_of_mem; i++)
    {
        if(room_list[current].member_list[i] == online_users[user_id].user_id)
        {
            for(; i < room_list[current].num_of_mem; i++)
            {
                room_list[current].member_list[i] = room_list[current].member_list[i + 1];
            }

            room_list[current].num_of_mem--;
            break;
        }
    }

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
    room_list[id].num_of_mem = -1;
    
    // end of critical section

    return 0;
}

/*  현재 존재하는 방의 개수 탐색
    return value    ret - 존재하는 방의 개수
*/
int current_room_num(void)                              // 현재 존재하는 방 개수 | room_list access (need mutex)
{
    int i, ret;

    // critical section

    for(i = 0, ret = 0; i < MAX_SIZE; i++)
    {
        if(room_list[i].num_of_mem != -1)
        {
            ret++;
        }
    }

    // end of critical section

    return ret;
}

/*  room_list 배열에서 비어있는 방 탐색
    return value    ret - 새로운 방 생성이 가능한 index
                    MAX_ROOM_SIZE - 더이상 방 생성이 불가능 할때
*/              

int find_empty_room(void)
{
    int ret;

    for(ret = 0; ret < MAX_ROOM_SIZE; ret++)
    {
        if(room_list[ret].num_of_mem == -1)
        {
            return ret;
        }
    }
    
    return MAX_ROOM_SIZE;
}