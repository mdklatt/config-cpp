#include "streambuf.hpp"
#include <stdexcept>


using configure::StreamBuffer;
using std::map;
using std::string;

configure::StreamBuffer::StreamBuffer(std::streambuf* source, const map<string, string>& params):
        params{params},
        source{source},
        buffer(buflen) {
    if (not params.empty()) {
        throw std::runtime_error{"parameter substitution not implemented"};
    }
    pubimbue(source->getloc());
    setg(buffer.data(), buffer.data(), buffer.data());
}


StreamBuffer::int_type StreamBuffer::underflow() {
    const auto count{source->sgetn(buffer.data(), buflen)};
    setg(buffer.data(), buffer.data(), buffer.data() + count);
    return count == 0 ? eof : sgetc();
}
