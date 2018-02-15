#pragma once
#include <NetworkConnection.h>
#include <list>
#include <memory>
#include <queue>



namespace GEM
{
	

	/**!
	Recives connection and make them ready for transmission
	*/
	class ConnectionController
	{
	private:
		//std::list<std::unique_ptr<NetworkConnection>> m_connections;
		boost::asio::io_context m_io_context;
		std::queue<std::unique_ptr<NetworkConnection>> m_newConnections;

		boost::asio::ip::tcp::acceptor m_acceptor;

		void Do_accept()
		{
			m_acceptor.async_accept([this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
			{
				if (!ec)
				{
					m_newConnections.emplace(std::make_unique<NetworkConnection>(std::move(socket)));
				}
				Do_accept();
			});
		}


	public:

		/**!
		Returns last new recived connection, if there is any, otherwise nullptr
		Don't forget to call ProcessConnection for recived connection. Otherwise they would not send or recive
		anything!
		*/
		std::unique_ptr<NetworkConnection> GetNewConnectionIfAny()
		{
			if (m_newConnections.empty()) { return nullptr; }
			auto Ptr= std::move(m_newConnections.front());
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
		}
	};
}