/*****************************************************************************
**
** 	Copyright (C) 2017 liushixiongcpp@163.com
**	All rights reserved.
**
*****************************************************************************/

#include "client_sock.h"

#include <iostream>

using std::cout;
using std::endl;

const string ip = "127.0.0.1";
const int port	= 8002;

int main(int argc, char *argv[])
{
	
	ClientSock client(ip, port);	

	client.SendMessage("this is test dome.");

	return 0;
}


