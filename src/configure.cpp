/**
 * Implementation of the configure module.
 */
#include "config/configure.hpp"
#include <cstdint>
#include <istream>
#include <stdexcept>


using std::invalid_argument;
using std::istream;
using std::string;

using namespace configure;


void Config::load(istream& stream, const std::string& root) {
    load(parse(stream), root);
}


void Config::load(const std::filesystem::path& path, const std::string& root) {
    load(parse(path.string()), root);
}


double& Config::as_real(const std::string &key) {
    return at<double>(key, toml::node_type::floating_point);
}


const double& Config::as_real(const std::string &key) const {
    return at<double>(key, toml::node_type::floating_point);
}


double Config::as_real(const std::string &key, const double& fallback) const {
    return at<double>(key, toml::node_type::floating_point, fallback);
}


int64_t& Config::as_integer(const std::string &key) {
    return at<int64_t>(key, toml::node_type::integer);
}


const int64_t& Config::as_integer(const std::string& key) const {
    return at<int64_t>(key, toml::node_type::integer);
}


int64_t Config::as_integer(const std::string& key, const int64_t& fallback) const {
    return at<int64_t>(key, toml::node_type::integer, fallback);
}


string& Config::as_string(const std::string& key) {
    return at<std::string>(key, toml::node_type::string);
}


const string& Config::as_string(const std::string &key) const {
    return at<std::string>(key, toml::node_type::string);
}


string Config::as_string(const std::string &key, const std::string &fallback) const {
    return at<std::string>(key, toml::node_type::string, fallback);
}


bool Config::has_key(const std::string& key) const {
    return tree.at_path(key).type() != toml::node_type::none;
}


void Config::load(const toml::table&& table, const std::string& root) {
    if (root.empty()) {
        tree = table;
    }
    else {
        insert_table(root) = table;
    }
}


template <typename T>
T& Config::at(const std::string& key, const toml::node_type& type) {
    auto node{tree.at_path(key)};
    if (node.type() != type) {
        insert<T>(key);
        node = tree.at_path(key);  // need to reassign to new node
    }
    return node.ref<T>();
}


template <typename T>
const T& Config::at(const std::string& key, const toml::node_type& type) const {
    const auto node{tree.at_path(key)};
    if (node.type() != type) {
        throw invalid_argument{"incorrect type for node '" + key + "'"};
    }
    return node.ref<T>();
}


template <typename T>
T Config::at(const std::string& key, const toml::node_type& type, const T& fallback) const {
    const auto node{tree.at_path(key)};
    return (node.type() == toml::node_type::none) ? fallback : at<T>(key, type);
}


template <typename T>
void Config::insert(const std::string& key) {
    const auto pos{key.rfind(keydel)};
    const auto parent{pos == string::npos ? "" : key.substr(0, pos)};
    const auto leaf{pos == string::npos ? key : key.substr(pos + 1)};
    toml::table& root{parent.empty() ? tree : insert_table(parent)};
    if (root[leaf].type() != toml::node_type::none) {
        throw invalid_argument{"incorrect type for node '" + key + "'"};
    }
    root.emplace(leaf, toml::value<T>{});
}


toml::table& Config::insert_table(const std::string& key) {
    std::string parent;
    for (auto it{key.begin()}; it != key.end(); ++it) {
        // Create parent nodes.
        if (*it == keydel) {
            parent.assign(key.begin(), it);
            if (tree.at_path(parent).type() == toml::node_type::none) {
                tree.emplace(parent, toml::table());
            }
            else if (not tree.at_path(parent).is_table()) {
                throw invalid_argument{"node '" + parent + "' is not a table"};
            }
            ++it;  // skip delimiter
        }
    }
    const auto pos{key.rfind(keydel)};
    const auto leaf{pos == string::npos ? key : key.substr(pos + 1)};
    if (parent.empty()) {
        tree.emplace(leaf, toml::table());
    }
    else {
        tree.at_path(parent).ref<toml::table>().emplace(leaf, toml::table());
    }
    return tree.at_path(key).ref<toml::table>();
}


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
