#include "stdafx.h"
#include "chat_server.h"


int chart_Test::Run()
{

	try
	{
	//	if (argc < 2)
	//	{
	//		std::cerr << "Usage: chat_server <port> [<port> ...]\n";
	//		return 1;
	//	}

		boost::asio::io_service io_service;

		chat_server_list servers;
		//for (int i = 1; i < argc; ++i)
		{
			using namespace std; // For atoi.
			tcp::endpoint endpoint(tcp::v4(), 13);
			chat_server_ptr server(new chat_server(io_service, endpoint));
			//chat_server_ptr server(new chat_server(io_service, endpoint)));
			
			servers.push_back(server);
		}
		boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));
		char line[chat_message::max_body_length + 1];


		while (std::cin.getline(line, chat_message::max_body_length + 1))
		{
			using namespace std; 
			// For strlen and memcpy.
			chat_message msg;
			msg.body_length(strlen(line));
			memcpy(msg.body(), line, msg.body_length());
			msg.encode_header();
			printf(line);
			chat_server_ptr sp_server = servers.back();
			sp_server->getRoom().deliver(msg);
			//sp_server->getRoom().
		}

		t.join();
		//io_service.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}



	return 0;
}
