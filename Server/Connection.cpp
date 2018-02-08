#include "stdafx.h"
#include "Connection.h"

namespace GEM
{
	void Connection::Send(std::stringstream & instream)
	{
		m_StreamToSend << instream.str();
	}
	std::stringstream & Connection::Recive()
	{
		return m_StreamToRecive;
	}
	void Connection::ClearReciveBuffer()
	{
		m_StreamToRecive.str(std::string());
	}
	void Connection::ProcessConnection()
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