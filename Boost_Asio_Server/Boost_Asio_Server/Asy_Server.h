#pragma once


#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#pragma warning (disable:4996)


using boost::asio::ip::udp;

inline std::string make_daytime_string()
{
	using namespace std; // For time_t, time and ctime;
	time_t now = time(0);
	return ctime(&now);
}

class udp_server
{
public:
	udp_server(boost::asio::io_service& io_service)
		: socket_(io_service, udp::endpoint(udp::v4(), 8013))
	{
		start_receive();
	}

private:
	void start_receive()
	{
		socket_.async_receive_from(
			boost::asio::buffer(recv_buffer_), remote_endpoint_,
			boost::bind(&udp_server::handle_receive, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	void handle_receive(const boost::system::error_code& error,
		std::size_t sz/*bytes_transferred*/)
	{
		if (!error || error == boost::asio::error::message_size)
		{
			//boost::shared_ptr<std::string> message(
			//	new std::string(make_daytime_string()));
			std::cout.write(recv_buffer_.data(), sizeof recv_buffer_.data());

			std::string msgdata ="this is the message";

			boost::shared_ptr<std::string> message(new std::string(""))/*("")*/;
				
			//message->assign();
			message->append(msgdata);
			//*message = boost::shared_ptr<std::string>("this is the message");

			socket_.async_send_to(boost::asio::buffer(*message), remote_endpoint_,
				boost::bind(&udp_server::handle_send, this, message,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));

			start_receive();
		}
	}

	void handle_send(boost::shared_ptr<std::string> /*message*/,
		const boost::system::error_code& /*error*/,
		std::size_t /*bytes_transferred*/)
	{
	}

	udp::socket socket_;
	udp::endpoint remote_endpoint_;
	boost::array<char, 4> recv_buffer_;
};

class Asy_Server
{
public:
	Asy_Server()
	{

	}

	~Asy_Server()
	{

	}

	int Run()
	{
		try
		{
			boost::asio::io_service io_service;
			udp_server server(io_service);
			io_service.run();
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}

		return 0;
	}
};