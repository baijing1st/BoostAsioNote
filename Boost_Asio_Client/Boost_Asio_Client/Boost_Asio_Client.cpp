// Boost_Asio_Client.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Sy_Clent.h"
#include "chat_client.h"


int main()
{
	//Sy_Clent sy;
	//sy.Run();

	chat_client_Test cct;
	cct.Run();

	getchar();
    return 0;
}

