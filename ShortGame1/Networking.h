#pragma once
#include <boost\asio.hpp>

namespace GEM
{
	class TCP_Connection
	{
	private:
		boost::asio::io_context m_io_context;

		boost::asio::ip::tcp::socket m_socket;
	public:
		TCP_Connection() : m_socket(m_io_context)
		{
			m_io_context.run();
			boost::system::error_code ec;
			m_socket.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 5543), ec);

			printf("%s", ec.message().c_str());
		}

		void Connect()
		{
		}
	};
}