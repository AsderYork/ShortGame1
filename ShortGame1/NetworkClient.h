#pragma once
#include <NetworkConnection.h>
#include <string>

namespace GEM
{
	class NetworkClient
	{
		static boost::asio::io_context m_io_context;

	public:

		static NetworkConnection ProduceConnection(std::string addr, unsigned int port)
		{
			auto Socket = boost::asio::ip::tcp::socket(m_io_context);
			Socket.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(addr), port));
			return NetworkConnection(std::move(Socket));
		}
		
	};
}