#include "config/toml.hpp"
#include <cstdint>
#include <istream>
#include <stdexcept>


using std::invalid_argument;
using std::istream;
using std::string;

using namespace configure;


TomlConfig::TomlConfig(istream& stream) {
    load(stream);
}


TomlConfig::TomlConfig(const std::filesystem::path& path) {
    load(path);
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
    return toml::parse(stream);
}


toml::table TomlConfig::parse(const std::filesystem::path& path) {
    return toml::parse_file(path.string());
}
