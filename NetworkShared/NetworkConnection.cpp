#include "stdafx.h"
#include "NetworkConnection.h"

namespace GEM
{

	NetworkConnection::NetworkConnection() {

	}

	std::string& NetworkConnection::SendData()
	{
		return m_dataToSend;
	}
	std::string& NetworkConnection::ReciveData()
	{
		return m_dataToRecive;
	}

	
	BufferIStream NetworkConnection::getReciveStream()
	{
		return BufferIStream(m_dataToRecive);
	}

	void NetworkConnection::ProcessConnection()
	{
		/*boost::system::error_code ec;
		auto Sended = m_socket.send(boost::asio::buffer(m_dataToSend), 0, ec);
		if (ec) { m_socket.close();  return; }

		m_dataToSend.erase(0, Sended);		
		std::size_t PreviousSize = m_dataToRecive.size();
		while (m_socket.available() != 0)
		{
			auto Avaliable = m_socket.available();
			PreviousSize = m_dataToRecive.size();
			m_dataToRecive.resize(m_dataToRecive.size() + Avaliable);
			boost::asio::read(m_socket, boost::asio::buffer(m_dataToRecive.data() + PreviousSize, Avaliable), boost::asio::transfer_exactly(Avaliable));
		}*/
	}
	bool NetworkConnection::isOpen()
	{
		return true;
		//return m_socket.is_open();
	}


	/*NetworkConnection::NetworkConnection(boost::asio::ip::tcp::socket&& socket) : m_socket(std::move(socket))
	{
	}*/
}