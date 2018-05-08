#pragma once
#include <streambuf>
#include <istream>

namespace GEM
{
	class BufferIStream : public std::istream
	{
	private:
		class char_array_buffer : public std::streambuf {
		public:
			char_array_buffer(const char *data, unsigned int len);

		private:
			friend class BufferIStream;

			int_type underflow();
			int_type uflow();
			int_type pbackfail(int_type ch);
			std::streamsize showmanyc();
			int CharsRead;

			const char * const begin_;
			const char * const end_;
			const char * current_;
		};

		char_array_buffer streambuf;
		std::string* buf;
	public:
		BufferIStream(std::string& _buf);
		BufferIStream(BufferIStream&& other);
		~BufferIStream();
	};
}