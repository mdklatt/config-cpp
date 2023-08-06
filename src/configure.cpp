/**
 * Implementation of the configure module.
 */
#include "config/configure.hpp"
#include <istream>
#include <stdexcept>


using std::invalid_argument;
using std::istream;
using std::string;

using namespace configure;


void Config::load(istream& stream, const string& root) {
    load(parse(stream), root);
}


void Config::load(const std::filesystem::path& path, const string& root) {
    load(parse(path.string()), root);
}


template <>
bool& Config::at<>(const string& key) {
    return at<bool>(key, toml::node_type::boolean);
}


template <>
double& Config::at<>(const string& key) {
    return at<double>(key, toml::node_type::floating_point);
}


template <>
long long& Config::at<>(const string& key) {
    return at<long long>(key, toml::node_type::integer);
}


template <>
string& Config::at<>(const string& key) {
    return at<string>(key, toml::node_type::string);
}


template <>
const string& Config::at<>(const string& key) const {
    return at<string>(key, toml::node_type::string);
}


bool Config::has_key(const string& key) const {
    return tree.at_path(key).type() != toml::node_type::none;
}


void Config::load(const toml::table&& table, const string& root) {
    if (root.empty()) {
        tree = table;
    }
    else {
        insert_table(root) = table;
    }
}


template <typename T>
T& Config::at(const string& key, const toml::node_type& type) {
    auto node{tree.at_path(key)};
    if (node.type() != type) {
        insert<T>(key);
        node = tree.at_path(key);  // need to reassign to new node
    }
    return node.ref<T>();
}


template <typename T>
const T& Config::at(const string& key, const toml::node_type& type) const {
    const auto node{tree.at_path(key)};
    if (node.type() != type) {
        throw invalid_argument{"incorrect type for node '" + key + "'"};
    }
    return node.ref<T>();
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


toml::table& Config::insert_table(const string& key) {
    string parent;
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


toml::table TomlConfig::parse(istream& stream) {
    return toml::parse(stream);
}


toml::table TomlConfig::parse(const std::filesystem::path& path) {
    return toml::parse_file(path.string());
}
