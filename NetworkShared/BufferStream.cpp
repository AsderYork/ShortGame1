#include "stdafx.h"
#include "BufferStream.h"
namespace GEM
{
	BufferIStream::char_array_buffer::char_array_buffer(const char *data, unsigned int len)
		: begin_(data), end_(data + len), current_(data), CharsRead(0) { }

	BufferIStream::char_array_buffer::int_type BufferIStream::char_array_buffer::underflow() {
		if (current_ == end_) {
			return traits_type::eof();
		}
		return traits_type::to_int_type(*current_);     // HERE!
	}

	BufferIStream::char_array_buffer::int_type BufferIStream::char_array_buffer::uflow() {
		if (current_ == end_) {
			return traits_type::eof();
		}
		CharsRead++;
		return traits_type::to_int_type(*current_++);   // HERE!
	}

	BufferIStream::char_array_buffer::int_type BufferIStream::char_array_buffer::pbackfail(int_type ch) {
		if (current_ == begin_ || (ch != traits_type::eof() && ch != current_[-1])) {
			return traits_type::eof();
		}
		CharsRead--;
		return traits_type::to_int_type(*--current_);   // HERE!
	}

	std::streamsize BufferIStream::char_array_buffer::showmanyc() {
		return end_ - current_;
	}
	BufferIStream::BufferIStream(std::string & _buf) : buf(&_buf), streambuf(_buf.data(), _buf.size()), std::istream(&streambuf) {}

	BufferIStream::BufferIStream(BufferIStream && other) : buf(other.buf), std::istream(static_cast<std::istream&&>(std::move(other))),
		streambuf(std::move(other.streambuf))
	{
		other.buf = nullptr;
	}

	BufferIStream::~BufferIStream()
	{
		if (buf != nullptr) {
			buf->erase(0, streambuf.CharsRead);
		}
	}
}