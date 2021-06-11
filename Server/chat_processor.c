#include "server_header.h"
#include "packet_header.h"

int add_block_list(int user_id, char *user_name)
{
    int num;
    int id = search_user(user_name);
    packet_blocked packet;
    
    packet.type = BLOCK;
    packet.size = sizeof(packet_blocked);
    packet.user_id = id;

    if(id != -1)
    {
        if(id == online_users[user_id].user_id)
        {
            packet.accept = DATA_DUPLICATE;
        }
        else
        {
            num = online_users[user_id].blocked_user_num;
            online_users[user_id].block_list[num] = id;
            online_users[user_id].blocked_user_num++;
            
            packet.accept = true;
        }
    }
    else
    {
        packet.accept = false;
    }
    
    packet_send(user_id, (char *)&packet);
    
    return 0;
}

int echo_message(int user_id, int room_number, char *message)
{
    int i, j;
    int target_id;
    packet_echo packet;

    strcpy(packet.buf, message);
    strcpy(packet.user_name, online_users[user_id].id);
    packet.type = CHAT;
    packet.size = sizeof(packet_echo);

    WaitForSingleObject(hMutex, INFINITE);
    for (i = 0; i < room_list[room_number].num_of_mem; i++)
    {
        target_id = room_list[room_number].member_list[i];
        if (online_users[target_id].blocked_user_num != 0)
        {
            for (j = 0; j < online_users[target_id].blocked_user_num; j++)
            {
                if (online_users[user_id].user_id != online_users[target_id].block_list[j])
                {
                    if (online_users[target_id].is_online)
                    {
                        packet_send(target_id, (char *)&packet);
                    }
                }
            }
        }
        else
        {
            if (online_users[target_id].is_online)
            {
                packet_send(target_id, (char *)&packet);
            }
        }
        
    }
    ReleaseMutex(hMutex);
    return 0;
}