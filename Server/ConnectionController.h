#pragma once
#include <NetworkConnection.h>
#include <list>
#include <memory>
#include <queue>



namespace GEM
{
	


	class ConnectionController
	{
	private:
		std::list<std::unique_ptr<NetworkConnection>> m_connections;
		boost::asio::io_context m_io_context;
		std::queue<NetworkConnection*> m_newConnections;

		boost::asio::ip::tcp::acceptor m_acceptor;

		void Do_accept()
		{
			m_acceptor.async_accept([this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
			{
				printf("Yelp!\n");
				if (!ec)
				{
					m_newConnections.emplace(m_connections.emplace_back(std::make_unique<NetworkConnection>(std::move(socket))).get());
				}

				Do_accept();
			});
		}


	public:
		
		const std::list<std::unique_ptr<NetworkConnection>>& ConnectionList()
		{
			return m_connections;
		}

		/**!
		Returns last new recived connection, if there is any, otherwise nullptr
		*/
		NetworkConnection* GetNewConnectionIfAny()
		{
			if (m_newConnections.empty()) { return nullptr; }
			auto Ptr= m_newConnections.front();
			m_newConnections.pop();
			return Ptr;
		}


		ConnectionController(unsigned int Port) : m_acceptor(m_io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), Port))
		{
			Do_accept();
		}

		void RunALittle()
		{
			m_io_context.run_for(std::chrono::milliseconds(1));
			for (auto& con : m_connections)
			{
				con->ProcessConnection();
			}
		}
	};
}