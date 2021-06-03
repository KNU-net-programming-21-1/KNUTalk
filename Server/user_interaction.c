#include "server_header.h"
#include <string.h>

int login(int user_id, char *ID, char *PW)                  // 서버에 로그인
{
    int result = search_user(ID);

    if(result >= 0)
    {
		if (!strcmp(registerd_users[result].pw, PW))			// 로그인 성공
		{
			registerd_users[result].memberInfo = online_users[user_id].memberInfo;
			registerd_users[result].is_online = true;
			online_users[user_id] = registerd_users[result];
		}
		else;													// 로그인 실패 -> error node 추가?
		return 0;
    }
    else
    {
        return SEARCH_ERROR;
    }
}

int logout(int user_id)                         // 서버에서 로그아웃
{
    int i;
    registerd_users[online_users[user_id].user_id].is_online = false;
    
    for(i = user_id; i < MAX_SIZE; i++)
    {
        online_users[i] = online_users[i + 1];
    }
    
    return 0;
}

/*	회원 가입 함수
	create new member structure
				&
	add to member_list
*/
int member_register(char *ID, char *PW)         // 서버에 계정 등록
{
     
    if (exist_user(ID))                   
    {
        return error_handling(DATA_DUPLICATE);
    }
	else
    {
		int ret_num = registerd_user();
		member *new_user;
		new_user = (member *)malloc(sizeof(member));
		//memset

		strcpy(new_user->id, ID);
		strcpy(new_user->id, PW);
		new_user->user_id = ret_num;

		registerd_users[ret_num] = *new_user;

		return 0;
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
        if(registerd_users[ret].user_id == -1)
        {
            break;
        }
    }

    return ret;
}

/*  char id로 검색하여 user_id 반환
    return value    user_id - char id에 해당하는 유저의 index
                    -1      - 검색 실패
*/
int search_user(char *id)                       // char id로 int user_id 검색
{
	int user_id = -1;
	int ret_num = registerd_user();
	int i;

	for (i = 0; i < ret_num; i++)
	{
		if (!strcmp(registerd_users[i].id, id))
		{
			user_id = i;
			break;
		}
	}

    return user_id;
}