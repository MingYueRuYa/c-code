// socket_client.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "tcpclient.hpp"


int _tmain(int argc, _TCHAR* argv[])
{
    TcpClient().run("127.0.0.1", 9876);

    getchar();

    getchar();
	return 0;
}

