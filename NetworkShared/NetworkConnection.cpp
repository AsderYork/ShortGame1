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
		std::size_t BytesRead = 0;
		while (m_socket.available() != 0)
		{
			auto Avaliable = m_socket.available();
			std::vector<char> VecChar;
			VecChar.resize(Avaliable);
			BytesRead += boost::asio::read(m_socket, boost::asio::buffer(VecChar.data(), Avaliable), boost::asio::transfer_exactly(Avaliable));
			Recive += std::string(VecChar.begin(), VecChar.end());
		}

		m_StreamToRecive.str(m_StreamToRecive.str() + Recive.substr(0, BytesRead));
	}
	bool NetworkConnection::isOpen()
	{
		return m_socket.is_open();
	}


	NetworkConnection::NetworkConnection(boost::asio::ip::tcp::socket&& socket) : m_socket(std::move(socket))
	{
	}
}