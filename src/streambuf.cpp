#include "streambuf.hpp"
#include <regex>
#include <stdexcept>

using configure::StreamBuffer;
using std::map;
using std::regex;
using std::runtime_error;
using std::smatch;
using std::string;


configure::StreamBuffer::StreamBuffer(std::streambuf* source, const map<string, string>& params):
    params{params},
    source{source} {
    pubimbue(source->getloc());
    setg(buffer.data(), buffer.data(), buffer.data());
}


StreamBuffer::int_type StreamBuffer::underflow() {
    buffer.resize(buflen);
    count = source->sgetn(buffer.data(), buflen);
    buffer.resize(count);
    if (not params.empty()) {
        replace();
    }
    if (buffer.empty() and not remainder.empty()) {
        buffer = remainder;
        remainder.clear();
    }
    if (buffer.empty() and count == 0) {
        return eof;
    }
    setg(buffer.data(), buffer.data(), buffer.data() + buffer.length());
    return sgetc();
}


void StreamBuffer::replace() {
    buffer = remainder + buffer;
    smatch match;
    for (const auto& item: params) {
        const regex key_re{R"(\$\{)" + item.first + R"(\})"};
        while (regex_search(buffer, match, key_re)) {
            buffer.replace(match.position(0), match.length(0), item.second);
        }
    }
    const auto pos{buffer.rfind('$')};
    if (pos != string::npos) {
        // Incomplete substitution
        remainder = buffer.substr(pos);
        buffer.resize(pos);
    }
}
