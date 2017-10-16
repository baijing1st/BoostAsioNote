#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <vector>
#include "connection.h" // Must come before boost/serialization headers.
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/thread/thread.hpp>
#include "stock.h"
#include "Data_Protocol.h"

namespace s11n_example {

	/// Serves stock quote information to any client that connects to it.
	class server
	{
	public:
		/// Constructor opens the acceptor and starts waiting for the first incoming
		/// connection.
		server(boost::asio::io_service& io_service, unsigned short port)
			: acceptor_(io_service,
				boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
		{
			// Create the data to be sent to each client.
			std::vector<stock> stocks_;
			stock s;
			s.code = "ABC";
			s.name = "A Big Company";
			s.open_price = 4.56;
			s.high_price = 5.12;
			s.low_price = 4.33;
			s.last_price = 4.98;
			s.buy_price = 4.96;
			s.buy_quantity = 1000;
			s.sell_price = 4.99;
			s.sell_quantity = 2000;
			stocks_.push_back(s);
			s.code = "DEF";
			s.name = "Developer Entertainment Firm";
			s.open_price = 20.24;
			s.high_price = 22.88;
			s.low_price = 19.50;
			s.last_price = 19.76;
			s.buy_price = 19.72;
			s.buy_quantity = 34000;
			s.sell_price = 19.85;
			s.sell_quantity = 45000;
			stocks_.push_back(s);

			std::ostringstream archive_stream;
			boost::archive::text_oarchive archive(archive_stream);
			archive << stocks_;
			std::string outbound_data_ = archive_stream.str();

			Com_Protocol::Package p;
			p.itype = 1;
			p.data = outbound_data_;

			this->package_ = p;


			// Start an accept operation for a new connection.
			connection_ptr new_conn(new connection(acceptor_.get_io_service()));

			//conns.push_back(new_conn);

			acceptor_.async_accept(new_conn->socket(),
				boost::bind(&server::handle_accept, this,
					boost::asio::placeholders::error, new_conn));
		}

		/// Handle completion of a accept operation.
		void handle_accept(const boost::system::error_code& e, connection_ptr conn)
		{
			if (!e)
			{
				conn->async_read(package_,
					boost::bind(&server::handle_read, this,
						boost::asio::placeholders::error, conn));
				
			}
			else
			{
				std::cerr << e.message() << std::endl;
			}

			// Start an accept operation for a new connection.
			connection_ptr new_conn(new connection(acceptor_.get_io_service()));

			//conns.push_back(new_conn);

			acceptor_.async_accept(new_conn->socket(),
				boost::bind(&server::handle_accept, this,
					boost::asio::placeholders::error, new_conn));
		}

		/// Handle completion of a write operation.
		void handle_write(const boost::system::error_code& e, connection_ptr conn)
		{

			// Nothing to do. The socket will be closed automatically when the last
			// reference to the connection object goes away.
			if (!e)
			{
				std::cout << "Data returned\n";
			}
			else
			{
				std::cerr << e.message() << std::endl;
			}
			
		}

		void handle_read(const boost::system::error_code& e, connection_ptr conn)
		{

			// Successfully accepted a new connection. Send the list of stocks to the
			// client. The connection::async_write() function will automatically
			// serialize the data structure for us.
			if (!e)
			{
				if (package_.itype == 1)
				{
					std::vector<stock> stocks_;
					std::string archive_data(package_.data);
					std::istringstream archive_stream(archive_data);
					boost::archive::text_iarchive archive(archive_stream);
					archive >> stocks_;

					// Print out the data that was received.
					for (std::size_t i = 0; i < stocks_.size(); ++i)
					{
						std::cout << "Stock number " << i << "\n";
						std::cout << "  code: " << stocks_[i].code << "\n";
						std::cout << "  name: " << stocks_[i].name << "\n";
						std::cout << "  open_price: " << stocks_[i].open_price << "\n";
						std::cout << "  high_price: " << stocks_[i].high_price << "\n";
						std::cout << "  low_price: " << stocks_[i].low_price << "\n";
						std::cout << "  last_price: " << stocks_[i].last_price << "\n";
						std::cout << "  buy_price: " << stocks_[i].buy_price << "\n";
						std::cout << "  buy_quantity: " << stocks_[i].buy_quantity << "\n";
						std::cout << "  sell_price: " << stocks_[i].sell_price << "\n";
						std::cout << "  sell_quantity: " << stocks_[i].sell_quantity << "\n";
					}

					conn->async_write(package_,
						boost::bind(&server::handle_write, this,
							boost::asio::placeholders::error, conn));

					std::cout << "Data Read\n";
				}
			}
			else
			{
				std::cerr << e.message() << std::endl;
			}
			if (!e)
			{
				conn->async_read(package_,
					boost::bind(&server::handle_read, this,
						boost::asio::placeholders::error, conn));

			}
			else
			{
				std::cerr << e.message() << std::endl;
			}
		
		}

		void write(const boost::system::error_code& e)
		{

		}

	private:
		/// The acceptor object used to accept incoming socket connections.
		boost::asio::ip::tcp::acceptor acceptor_;

		/// The data to be sent to each client.
		//std::vector<stock> stocks_;
		Com_Protocol::Package package_;

		//std::vector<connection_ptr> conns;


	};

} // namespace s11n_example

class Ser_Server_Test
{
public:
	int Run()
	{
		try
		{
			//// Check command line arguments.
			//if (argc != 2)
			//{
			//	std::cerr << "Usage: server <port>" << std::endl;
			//	return 1;
			//}

			unsigned short port = boost::lexical_cast<unsigned short>("8013");

			boost::asio::io_service io_service;
			s11n_example::server server(io_service, port);


			boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));

			char line[10];
			while (std::cin.getline(line, 11))
			{
				using namespace std; // For strlen and memcpy.
				//if (!strcmp("1", line))
				//{
				//	std::vector<s11n_example::stock> stocks_;
				//	s11n_example::stock s;
				//	s.code = "ABC";
				//	s.name = "A Big Company";
				//	s.open_price = 4.56;
				//	s.high_price = 5.12;
				//	s.low_price = 4.33;
				//	s.last_price = 4.98;
				//	s.buy_price = 4.96;
				//	s.buy_quantity = 1000;
				//	s.sell_price = 4.99;
				//	s.sell_quantity = 2000;
				//	stocks_.push_back(s);
				//	s.code = "DEF";
				//	s.name = "Developer Entertainment Firm";
				//	s.open_price = 20.24;
				//	s.high_price = 22.88;
				//	s.low_price = 19.50;
				//	s.last_price = 19.76;
				//	s.buy_price = 19.72;
				//	s.buy_quantity = 34000;
				//	s.sell_price = 19.85;
				//	s.sell_quantity = 45000;
				//	stocks_.push_back(s);

				//	std::ostringstream archive_stream;
				//	boost::archive::text_oarchive archive(archive_stream);
				//	archive << stocks_;
				//	std::string outbound_data_ = archive_stream.str();

				//	Com_Protocol::Package p;
				//	p.itype = 1;
				//	p.data = outbound_data_;

				//	/*conn->async_write(p,
				//		boost::bind(&server::handle_write, this,
				//			boost::asio::placeholders::error, conn));*/


				//}
			}

			//io_service.run();
			t.join();
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}

		return 0;
	}

};


