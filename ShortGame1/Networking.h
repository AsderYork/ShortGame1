#pragma once
#include <boost\asio.hpp>
#include <sstream>

namespace GEM
{
	class TCP_Connection
	{
	private:
		boost::asio::io_context m_io_context;

		boost::asio::ip::tcp::socket m_socket;


		std::stringstream m_StreamToSend;
		std::stringstream m_StreamToRecive;


	public:
		TCP_Connection(int Port) : m_socket(m_io_context)
		{
			m_io_context.run();
			boost::system::error_code ec;
			m_socket.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), Port), ec);

			printf("%s", ec.message().c_str());
		}


		/**!
		Add the contents of a stream to the output of the socket.
		The data will be actually sent only with the next call to ProcessConnection
		*/
		void Send(std::stringstream& instream);

		/**!
		Provides access to a recive buffer.
		There is no guarantee, that caller wouldn't alterate the state of the buffer
		so it's adviced to use this method exclusively from one place.

		*/
		std::stringstream& Recive();

		/**!
		Clears recive buffer.
		*/
		void ClearReciveBuffer();

		/**!
		Recives everything, that should be recived, sends everything, that should be send
		*/
		void ProcessConnection();
	};
		
}