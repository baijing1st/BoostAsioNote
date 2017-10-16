#pragma once
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <vector>
#include "connection.h" // Must come before boost/serialization headers.
#include <boost/serialization/vector.hpp>
#include <boost/thread/thread.hpp>
#include "stock.h"
#include "Data_Protocol.h"


namespace s11n_example {

	/// Downloads stock quote information from a server.
	class client
	{
	public:
		/// Constructor starts the asynchronous connect operation.
		client(boost::asio::io_service& io_service,
			const std::string& host, const std::string& service)
			: connection_(io_service)
		{
			// Resolve the host name into an IP address.
			boost::asio::ip::tcp::resolver resolver(io_service);
			boost::asio::ip::tcp::resolver::query query(host, service);
			boost::asio::ip::tcp::resolver::iterator endpoint_iterator =
				resolver.resolve(query);

			// Start an asynchronous connect operation.
			boost::asio::async_connect(connection_.socket(), endpoint_iterator,
				boost::bind(&client::handle_connect, this,
					boost::asio::placeholders::error));
		}

		/// Handle completion of a connect operation.
		void handle_connect(const boost::system::error_code& e)
		{
			if (!e)
			{
				// Successfully established connection. Start operation to read the list
				// of stocks. The connection::async_read() function will automatically
				// decode the data that is read from the underlying socket.
				connection_.async_read(package_,
					boost::bind(&client::handle_read, this,
						boost::asio::placeholders::error));
			}
			else
			{
				// An error occurred. Log it and return. Since we are not starting a new
				// operation the io_service will run out of work to do and the client will
				// exit.
				std::cerr << e.message() << std::endl;
			}
		}

		/// Handle completion of a read operation.
		void handle_read(const boost::system::error_code& e)
		{
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
					package_.clear();

				}
				else if (package_.itype == 2)
				{
				}
				else if (package_.itype == 3)
				{
				}

				// Successfully established connection. Start operation to read the list
				// of stocks. The connection::async_read() function will automatically
				// decode the data that is read from the underlying socket.
				connection_.async_read(package_,
					boost::bind(&client::handle_read, this,
						boost::asio::placeholders::error));
			}
			else
			{
				// An error occurred.
				std::cerr << e.message() << std::endl;
			}

			// Since we are not starting a new operation the io_service will run out of
			// work to do and the client will exit.


		}

		void write(Com_Protocol::Package package)
		{
			connection_.async_write(package,
				boost::bind(&client::handle_write, this,
					boost::asio::placeholders::error));
		}

		void handle_write(const boost::system::error_code& e)
		{
			// Nothing to do. The socket will be closed automatically when the last
			// reference to the connection object goes away.
			if (e)
			{
				std::cout << e.message() << std::endl;
			}
			
			std::cout << "Data Sent\n";
		}

		void close()
		{
			connection_.close();
		}

	private:
		/// The connection to the server.
		connection connection_;

		/// The data received from the server.
		//std::vector<stock> stocks_;

		Com_Protocol::Package package_;

		
	};

} // namespace s11n_example

//inline void run1(boost::asio::io_service &io_service){
//	io_service.run();
//}

class Ser_Client_Test
{
public:
	int Run()
	{
		try
		{
			boost::asio::io_service io_service;
			boost::asio::io_service::work m_work(io_service);
			s11n_example::client *client = new s11n_example::client(io_service, "127.0.0.1", "8013");
			//boost::thread t(boost::bind(run1,boost::ref(io_service)));
			boost::thread t;
			t= boost::thread(boost::bind(&boost::asio::io_service::run, &io_service));
			
			char line[10];
			while (std::cin.getline(line, 11))
			{
				using namespace std; // For strlen and memcpy.
				if (!strcmp("1" ,  line) )
				{
					std::vector<s11n_example::stock> stocks_;
					s11n_example::stock s;
					s.code = "GGG";
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
					s.code = "HHH";
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

					client->write(p);
				}
			}

			client->close();
			t.join();
			
			io_service.stop();
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}

		return 0;
	}
};


