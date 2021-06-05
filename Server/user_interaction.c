#include "server_header.h"
#include "packet_header.h"
#include <string.h>

int login(int user_id, char *ID, char *PW)                  // 서버에 로그인
{
    int result = search_user(ID);
    packet_accept packet;

    packet.size = sizeof(packet_accept);
    packet.type = LOGIN;

    if(result >= 0)
    {
		if (!strcmp(registered_users[result].pw, PW))			// 로그인 성공
		{
			registered_users[result].memberInfo = online_users[user_id].memberInfo;
			registered_users[result].is_online = true;
			online_users[user_id] = registered_users[result];

            packet.accept = true;
		}
		else													// 로그인 실패 -> error node 추가?
        {
            packet.accept = false;
        }
    }
    else
    {
        packet.accept = false;
    }
    
    packet_send(user_id, &packet);

    return 0;
}

int logout(int user_id)                         // 서버에서 로그아웃
{
    packet_logout_accept packet;
    
    packet.accept = true;
    packet.size = sizeof(packet_logout_accept);
    packet.type = LOGOUT;

    registered_users[online_users[user_id].user_id].is_online = false;
    online_users[user_id].user_id = -1;
    packet_send(user_id, &packet);
    shutdown(online_users[user_id].memberInfo.s, SD_SEND);

    return 0;
}

/*	회원 가입 함수
    registered_users[] 에 새 유저의 user_id, ID, Password 등록
    return value    ret_num - 등록된 유저의 user_id(registered_users 배열 인덱스)
                    DATA_DUPLICATE - ID 중복
                    DATA_FAILURE - 잘못된 입력 양식
*/
int member_register(int user_id, char *ID, char *PW)         // 서버에 계정 등록
{
    int ret_num;
    int id_len, pw_len;
    packet_registered packet;

    packet.size = sizeof(packet_registered);
    packet.type = REGISTER;

    id_len = strlen(ID);
    pw_len = strlen(PW);

    if(search_user(ID) != -1)
    {
        packet.accept = DATA_DUPLICATE;
        packet_send(user_id, &packet);
        return error_handling(DATA_DUPLICATE);
    }
	else if(id_len > 4 && id_len < 20 && pw_len > 10 && pw_len < 20)
    {
		ret_num = registerd_user();

		strcpy(registered_users[ret_num].id, ID);
		strcpy(registered_users[ret_num].pw, PW);
		registered_users[ret_num].user_id = ret_num;
        registered_users[ret_num].is_online = false;     // 등록 성공할 경우 초기화면으로 돌아가 로그인 필요

        packet.accept = true;
        packet_send(user_id, &packet);

		return ret_num;
    }
    else
    {
        packet.accept = DATA_FAILURE;
        packet_send(user_id, &packet);
        return error_handling(DATA_FAILURE);
    }
}

/*  등록된 유저 수 반환 함수
    return value    ret - 등록된 유저 수
*/
int registerd_user(void)
{
    int ret;

    for(ret = 0; ret < MAX_SIZE; ret++)
    {
        if(registered_users[ret].user_id == -1)
        {
            break;
        }
    }

    return ret;
}

/*  char name으로 검색하여 user_id 반환
    return value    user_id - char id에 해당하는 유저의 index
                    -1      - 검색 실패
*/
int search_user(char *name)                       // char id로 int user_id 검색
{
	int user_id = -1;
	int ret_num = registerd_user();
	int i;

	for (i = 0; i < ret_num; i++)
	{
		if (!strcmp(registered_users[i].id, name))
		{
			user_id = i;
			break;
		}
	}

    return user_id;
}