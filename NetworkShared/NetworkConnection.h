#pragma once
#ifndef _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#endif // !_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#include "BufferStream.h"

//#include <boost\asio.hpp>
#include <sstream>
#include <cereal\cereal.hpp>
#include <cereal\archives\binary.hpp>

namespace GEM
{
	class NetworkConnection
	{
	private:
		//boost::asio::ip::tcp::socket m_socket;
		
		std::string m_dataToSend;
		std::string m_dataToRecive;

	public:
		//NetworkConnection(boost::asio::ip::tcp::socket&& socket);

		NetworkConnection();

		NetworkConnection(const NetworkConnection&) = delete;
		NetworkConnection(NetworkConnection&&) = default;

		
		template<typename...T>
		void SendAsArchive(T&&...args)
		{
			std::stringstream OutStre;
			cereal::BinaryOutputArchive ar(OutStre);
			ar(std::forward<T>(args)...);
			m_dataToSend += OutStre.str();
		}

		template<typename...T>
		void ReciveAsArchive(T&&...args)
		{
			BufferIStream Buff(m_dataToRecive);
			cereal::BinaryInputArchive ar(Buff);

			ar(std::forward<T>(args)...);
		}


		/**!
		Provides access to send buffer.
		Insert some data in it!
		*/
		std::string& SendData();
		/**!
		Provides access to recive buffer.
		Be carefult! It doesn't know, if you're reading something from it or not!
		So if you read some data from it, just use string::erase(0,ReadBytes) to remove part you just read
		*/
		std::string& ReciveData();

		/**!
		Returns an object, that contains and manages istream connected to a recive buffer.
		\warning NEVER call this method, if you're still having an instance of bufferIStream for this connection
		or in process of using ReciveAsArchive! This will lead to incorrect data removal from buffer
		*/
		BufferIStream getReciveStream();

		
		void ProcessConnection();

		/**!
		Check weather this connection is still open or not
		*/
		bool isOpen();
	};
}