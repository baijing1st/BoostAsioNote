// Boost_Asio_Server.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Asy_Server.h"
#include "chat_server.h"
#include "Serialization_server.h"


int main()
{
	//Asy_Server as;
	//as.Run();

	//chart_Test ct;
	//ct.Run();

	Ser_Server_Test sst;
	sst.Run();

    return 0;
}

