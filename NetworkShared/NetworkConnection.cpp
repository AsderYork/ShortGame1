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
		std::vector<char> VecChar;
		VecChar.resize(Avaliable);
		auto BytesRead = boost::asio::read(m_socket, boost::asio::buffer(VecChar.data(), Avaliable), boost::asio::transfer_exactly(Avaliable));

		Recive = std::string(VecChar.begin(), VecChar.end());
		m_StreamToRecive.str(m_StreamToRecive.str() + Recive.substr(0, BytesRead));
	}
}