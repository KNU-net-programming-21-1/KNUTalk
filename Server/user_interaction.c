#include "server_header.h"

int login(char *ID, char *PW)                  // 서버에 로그인
{
    if( /* user information is correct */ )
    {
        return true;
    }
    else if( /* incorrcet user information */ )
    {
        return false;
    }
    else /* cannot found user id */
    {
        return SEARCH_ERROR;
    }
}

int member_register(char *ID, char *PW)         // 서버에 계정 등록
{
    if( /* USER ID is already exist */ )
    {
        return error_handling(DATA_DUPLICATE);
    }
    else
    {
        /*
            create new member structure
                &
            add to member_list
        */
        return 0;
    }
}