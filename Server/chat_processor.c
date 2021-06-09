#include "server_header.h"
#include "packet_header.h"

int add_block_list(int user_id, char *user_name)
{
    int num, id;
    packet_blocked packet;

    packet.type = BLOCK;
    packet.size = sizeof(packet_blocked);
    
    id = search_user(user_name);
    
    if(id != -1)
    {
        num = online_users[user_id].blocked_user_num;
        online_users[user_id].block_list[num] = id;
        packet.accept = true;
    }
    else
    {
        packet.accept = false;
    }
    
    packet_send(user_id, &packet);
    
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

    for(i = 0; i < room_list[room_number].num_of_mem; i++)
    {
        target_id = room_list[room_number].member_list[i];
        for(j = 0; j < online_users[target_id].blocked_user_num; j++)
        {
            if(user_id != online_users[target_id].block_list[j])
            {
                if(online_users[target_id].is_online)
                {
                    if(target_id != user_id)
                    {
                        packet_send(target_id, &packet);
                    }
                }
                
            }
        }
    }

    return 0;
}