#pragma once
#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

class Sy_Clent
{
public:
	Sy_Clent();
	~Sy_Clent();
	int Run()
	{
		try
		{
			/*if (argc != 2)
			{
				std::cerr << "Usage: client <host>" << std::endl;
				return 1;
			}*/

			boost::asio::io_service io_service;

			udp::resolver resolver(io_service);
			udp::resolver::query query(udp::v4(), "127.0.0.1", /*"daytime"*/"8013");
			udp::endpoint receiver_endpoint = *resolver.resolve(query);

			udp::socket socket(io_service);
			socket.open(udp::v4());

			//boost::array<char, 1> send_buf = { { 0 } };
			boost::array<char, 4> send_buf = { { "233" } };

			//send_buf.assign("666");
			socket.send_to(boost::asio::buffer(send_buf), receiver_endpoint);

			boost::array<char, 128> recv_buf;
			udp::endpoint sender_endpoint;
			size_t len = socket.receive_from(
				boost::asio::buffer(recv_buf), sender_endpoint);

			std::cout.write(recv_buf.data(), len);
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}

		return 0;
	}
};

