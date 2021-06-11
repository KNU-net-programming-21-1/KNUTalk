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
        packet_send(user_id, (char *)&packet);
        return error_handling(LIMIT_REACHED + OFFSET);
    }
    else
    {
        // critical section
        WaitForSingleObject(hMutex, INFINITE);
        room_list[id].num_of_mem = 0;
        strcpy(room_list[id].room_name, name);
        for(i = 0; i < MAX_SIZE; i++)
        {
            room_list[id].member_list[i] = -1;
        }

        room_list[id].member_list[0] = online_users[user_id].user_id;
        
#if DEBUG == 1
        printf("Room Info\n");
        for (i = 1; i < find_empty_room(); i++)
        {
            printf("Room ID: %d, Room Name: %s\n",i,room_list[i].room_name);
        }
#endif
        ReleaseMutex(hMutex);
        // end of critical section

        packet.accept = true;
        packet.room_id = id;
        strcpy(packet.room_name, name);
        packet_send(user_id, (char *)&packet);

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
    int member_id;
    packet_join packet;

    packet.size = sizeof(packet_join);
    packet.type = ENTER;
    packet.room_id = room_id;

    // critical section
    WaitForSingleObject(hMutex, INFINITE);
    member_count = room_list[room_id].num_of_mem;
    ReleaseMutex(hMutex);
    // end of critical section

    if (member_count == -1)
    {
        packet.accept = false;
        packet_send(user_id, (char *)&packet);

        return error_handling(SEARCH_ERROR + OFFSET);
    }
    else if(member_count == MAX_SIZE)
    {
        packet.accept = false;
        packet_send(user_id, (char *)&packet);

        return error_handling(LIMIT_REACHED + OFFSET);
    }
    else
    {
        // critical section
        WaitForSingleObject(hMutex, INFINITE);
        room_list[room_id].member_list[member_count] = user_id;
        room_list[room_id].num_of_mem++;        
        online_users[user_id].cur_room = room_id;
        strcpy(packet.user_name, online_users[user_id].id);
        packet.accept = true;
        packet.room_id = room_id;

        for(i = 0; i < room_list[room_id].num_of_mem; i++)
        {
            member_id = room_list[room_id].member_list[i];
            if(online_users[member_id].cur_room == room_id) // 현재 방이 room_id인 유저에게만 패킷 전송
            {
                packet_send(member_id, (char *)&packet);
            }
            
        }
        ReleaseMutex(hMutex);
        // end of critical section

        return 0;
    }
}

/*  현재 참가 중인 방에서 퇴장
    cur_room 에서 나감.
    return value    0 - 정상 종료
*/
int quit_room(int room_id, int user_id)                        // 방 나가기
{
    int i;
    WaitForSingleObject(hMutex, INFINITE);
    for(i = 0; i < room_list[room_id].num_of_mem; i++)
    {
        if(room_list[room_id].member_list[i] == user_id)
        {
            for(; i < room_list[room_id].num_of_mem; i++)
            {
                room_list[room_id].member_list[i] = room_list[room_id].member_list[i + 1];
            }
            room_list[room_id].num_of_mem--;
            break;
        }
    }
    ReleaseMutex(hMutex);
    online_users[user_id].cur_room = 0;

    return 0;
}

/*  존재하는 방 삭제
*/
int delete_room(int room_id)                                 // 방 삭제 | room_list access (need mutex)
{
    int i;

    // critical section
    WaitForSingleObject(hMutex, INFINITE);
    strcpy(room_list[room_id].room_name, NULL);
    for(i = 0; i < room_list[room_id].num_of_mem; i++)
    {
        room_list[room_id].member_list[i] = -1;
    }
    room_list[room_id].num_of_mem = -1;
    ReleaseMutex(hMutex);
    // end of critical section

    return 0;
}

int room_info_request(int user_id)
{
    int i;
    packet_roominfo packet;
    packet.type = ROOMINFO;
    packet.size = sizeof(packet_roominfo);
    packet.room_id = 0;
    packet.member_count = 0;
    strcpy(packet.room_name, room_list[0].room_name);

    packet_send(user_id, (char *)&packet);

    WaitForSingleObject(hMutex, INFINITE);
    for(i = 1; i < MAX_SIZE; i++)
    {
        if (room_list[i].num_of_mem != -1)
        {
            packet.room_id = i;
            packet.member_count = room_list[i].num_of_mem;
            strcpy(packet.room_name, room_list[i].room_name);
            packet_send(user_id, (char *)&packet);
        }
    }
    ReleaseMutex(hMutex);
    return 0;
}


/*  현재 존재하는 방의 개수 탐색
    return value    ret - 존재하는 방의 개수
*/
int current_room_num(void)                              // 현재 존재하는 방 개수 | room_list access (need mutex)
{
    int i, ret;

    // critical section
    WaitForSingleObject(hMutex, INFINITE);
    for(i = 1, ret = 0; i < MAX_SIZE; i++)
    {
        if(room_list[i].num_of_mem != -1)
        {
            ret++;
        }
    }
    ReleaseMutex(hMutex);
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
    WaitForSingleObject(hMutex, INFINITE);
    for(ret = 1; ret < MAX_ROOM_SIZE; ret++)
    {
        if(room_list[ret].num_of_mem == -1)
        {
            return ret;
        }
    }
    ReleaseMutex(hMutex);
    return MAX_ROOM_SIZE;
}