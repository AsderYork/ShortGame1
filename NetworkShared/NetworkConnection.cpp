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
		auto Sended = m_socket.send(boost::asio::buffer(m_StreamToSend.str()));
		m_StreamToSend.str(m_StreamToSend.str().substr(Sended, std::string::npos));
		auto Avaliable = m_socket.available();
		std::string Recive;
		Recive.reserve(Avaliable);
		auto BytesRead = boost::asio::read(m_socket, boost::asio::buffer(Recive.data(), Avaliable), boost::asio::transfer_exactly(64));
		Recive = Recive.data();
		m_StreamToRecive.str(m_StreamToRecive.str() + Recive.substr(0, BytesRead));
	}
}