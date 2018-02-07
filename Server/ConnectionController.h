#pragma once
#include "Connection.h"
#include <list>
#include <memory>


#include <boost\asio.hpp>

namespace GEM
{
	class Connection
	{
	private:
		boost::asio::ip::tcp::socket m_socket;

	public:
		Connection(boost::asio::ip::tcp::socket&& socket) : m_socket(std::move(socket))
		{}
	};


	class ConnectionController
	{
	private:
		std::list<std::unique_ptr<Connection>> m_connections;
		boost::asio::io_context m_io_context;

		boost::asio::ip::tcp::acceptor m_acceptor;

		void Do_accept()
		{
			m_acceptor.async_accept([this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
			{
				printf("Yelp!\n");
				if (!ec)
				{
					m_connections.emplace_back(std::make_unique<Connection>(std::move(socket)));
				}

				Do_accept();
			});
		}


	public:


		ConnectionController(unsigned int Port) : m_acceptor(m_io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), Port))
		{
			Do_accept();
		}

		void RunALittle()
		{
			m_io_context.run();
			m_io_context.restart();
		}
	};
}