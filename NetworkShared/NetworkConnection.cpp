#include "stdafx.h"
#include "NetworkConnection.h"

namespace GEM
{
	void NetworkConnection::Send(std::stringstream & instream)
	{
		m_StreamToSend << instream.str();
	}
	std::stringstream & NetworkConnection::Recive()
	{
		return m_StreamToRecive;
	}
	void NetworkConnection::ClearReciveBuffer()
	{
		m_StreamToRecive.str(std::string());
	}
	void NetworkConnection::ProcessConnection()
	{
		boost::system::error_code ec;
		auto Sended = m_socket.send(boost::asio::buffer(m_StreamToSend.str()), 0, ec);
		if (ec) { m_socket.close();  return; }

		m_StreamToSend.str(m_StreamToSend.str().substr(Sended, std::string::npos));
		
		
		std::string Recive;
		std::size_t PreviousSize = 0;
		while (m_socket.available() != 0)
		{
			auto Avaliable = m_socket.available();
			PreviousSize = Recive.size();
			Recive.resize(Recive.size() + Avaliable);
			boost::asio::read(m_socket, boost::asio::buffer(Recive.data() + PreviousSize, Avaliable), boost::asio::transfer_exactly(Avaliable));
		}

		m_StreamToRecive.str(m_StreamToRecive.str() + Recive);
	}
	bool NetworkConnection::isOpen()
	{
		return m_socket.is_open();
	}


	NetworkConnection::NetworkConnection(boost::asio::ip::tcp::socket&& socket) : m_socket(std::move(socket))
	{
	}
}