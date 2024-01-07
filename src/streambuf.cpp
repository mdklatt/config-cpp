#include "streambuf.hpp"
#include <stdexcept>

using configure::StreamBuffer;
using std::map;
using std::runtime_error;
using std::string;


configure::StreamBuffer::StreamBuffer(std::streambuf* source, const map<string, string>& params):
    params{params},
    source{source} {
    pubimbue(source->getloc());
    setg(buffer.data(), buffer.data(), buffer.data());
}


StreamBuffer::int_type StreamBuffer::underflow() {
    buffer.resize(buflen);
    if (params.empty()) {
        const auto count{source->sgetn(buffer.data(), buflen)};
        buffer.resize(count);
    }
    else {
        replace();
    }
    setg(buffer.data(), buffer.data(), buffer.data() + buffer.length());
    return buffer.empty() ? eof : sgetc();
}


void StreamBuffer::replace() {
    string key;
    ssize_t count{0};
    const auto repl = [this, &count, &key]() {
        if (key.empty()) {
            return;
        }
        const auto& value = params.at(key);
        for (const auto c: value) {
            buffer[count++] = c;
        }
        key.clear();
    };
    bool open_param{false};
    bool open_brace{false};
    while (count < buflen) {
        // Continue until buflen characters have been read or while there is
        // still a parameter in progress.
        auto c{source->sbumpc()};
        if (c == eof) {
            // End of input.
            if (open_param) {
                // FIXME: Continue reading from stream.
                throw runtime_error{"incomplete parameter"};
            }
            break;
        }
        else if (c == '$') {
            const auto next{source->sbumpc()};
            if (next == '$') {
                // "$$" => literal '$'
                c = next;
            }
            if (next == '{') {
                open_param = true;
                open_brace = true;
            }
            else {
                // Start of a parameter sequence.
                if (open_param) {
                    throw std::runtime_error("unexpected $ in input");
                }
                open_param = true;
                source->sputbackc(next);
            }
        }
        else if ((c == '}' or (c == '\n')) and open_param) {
            if (c == '}' and not open_brace) {
                throw runtime_error{"unexpected '}' in input"};
            }
            repl();
            open_param = false;
            open_brace = false;
        }
        else if (open_param) {
            key += c;
        }
        else {
            buffer[count++] = c;
        }
    }
    buffer.resize(count);
}
