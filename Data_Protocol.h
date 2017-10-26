#pragma once

#include <string>

namespace Com_Protocol {

	//
	struct Package
	{
		int itype;
		std::string data;

		template <typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & itype;
			ar & data;
		}

		void clear()
		{
			itype = 0;
			data = "";
		}
	};

	//
	struct data1
	{
		int i;
		std::string d;

		template <typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & i;
			ar & d;
		}
	};

	/// Structure to hold information about a single stock.
	struct stock
	{
		std::string code;
		std::string name;
		double open_price;
		double high_price;
		double low_price;
		double last_price;
		double buy_price;
		int buy_quantity;
		double sell_price;
		int sell_quantity;

		template <typename Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar & code;
			ar & name;
			ar & open_price;
			ar & high_price;
			ar & low_price;
			ar & last_price;
			ar & buy_price;
			ar & buy_quantity;
			ar & sell_price;
			ar & sell_quantity;
		}
	};

} // namespace s11n_example
