#include "config/toml.hpp"
#include <istream>
#include <sstream>
#include <stdexcept>


using std::invalid_argument;
using std::istream;
using std::ostringstream;
using std::string;

using namespace configure;


TomlConfig::TomlConfig(istream& stream) {
    load(stream);
}


TomlConfig::TomlConfig(istream& stream, const Params& params) {
    load(stream, params);
}


TomlConfig::TomlConfig(const std::filesystem::path& path) {
    load(path);
}


TomlConfig::TomlConfig(const std::filesystem::path& path, const Params& params) {
    load(path, params);
}


bool& TomlConfig::as_boolean(const std::string &key) {
    return at<bool>(key, toml::node_type::boolean);
}


const bool& TomlConfig::as_boolean(const std::string &key) const {
    return at<bool>(key, toml::node_type::boolean);
}


bool TomlConfig::as_boolean(const std::string &key, const bool& fallback) const {
    return at<bool>(key, toml::node_type::boolean, fallback);
}


toml::table TomlConfig::parse(istream& stream) {
    // The toml::parse() function produces an empty table when the input
    // stream is backed by a StreamBuffer. No idea why this is happening
    // because StreamBuffer works fine with the file types. As a workaround,
    // read the input into a string first, then parse it.
    // FIXME: return toml::parse(stream);
    ostringstream buffer;
    buffer << stream.rdbuf();
    const auto conf{buffer.str()};
    return toml::parse(buffer.str());
}


toml::table TomlConfig::parse(const std::filesystem::path& path) {
    return toml::parse_file(path.string());
}
