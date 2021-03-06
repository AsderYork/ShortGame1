#pragma once
#include <boost\asio.hpp>
#include <sstream>

namespace GEM
{
	class Connection
	{
	private:
		boost::asio::ip::tcp::socket m_socket;

		std::stringstream m_StreamToSend;
		std::stringstream m_StreamToRecive;
	public:
		Connection(boost::asio::ip::tcp::socket&& socket) : m_socket(std::move(socket))
		{}

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

		void ProcessConnection();
	};
}