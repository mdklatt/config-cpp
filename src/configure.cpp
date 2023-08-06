/**
 * Implementation of the configure module.
 */
#include "config/configure.hpp"
#include <yaml-cpp/yaml.h>
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


void TomlConfig::load(istream& stream, const string& root) {
    const auto table{toml::parse(stream)};
    if (root.empty()) {
        tree = table;
    }
    else {
        insert_table(root) = table;
    }
}


void TomlConfig::load(const std::filesystem::path& path, const string& root) {
    const auto table{toml::parse_file(path.string())};
    if (root.empty()) {
        tree = table;
    }
    else {
        insert_table(root) = table;
    }
}


template <>
bool& TomlConfig::at<>(const string& key) {
    return at<bool>(key, toml::node_type::boolean);
}


template <>
double& TomlConfig::at<>(const string& key) {
    return at<double>(key, toml::node_type::floating_point);
}


template <>
long long& TomlConfig::at<>(const string& key) {
    return at<long long>(key, toml::node_type::integer);
}


template <>
string& TomlConfig::at<>(const string& key) {
    return at<string>(key, toml::node_type::string);
}


template <>
const string& TomlConfig::at<>(const string& key) const {
    return at<string>(key, toml::node_type::string);
}


bool TomlConfig::has_key(const string& key) const {
    return tree.at_path(key).type() != toml::node_type::none;
}


template <typename T>
T& TomlConfig::at(const string& key, const toml::node_type& type) {
    auto node{tree.at_path(key)};
    if (node.type() != type) {
        insert<T>(key);
        node = tree.at_path(key);  // need to reassign to new node
    }
    return node.ref<T>();
}


template <typename T>
const T& TomlConfig::at(const string& key, const toml::node_type& type) const {
    const auto node{tree.at_path(key)};
    if (node.type() != type) {
        throw invalid_argument{"incorrect type for node '" + key + "'"};
    }
    return node.ref<T>();
}


template <typename T>
void TomlConfig::insert(const std::string& key) {
    const auto pos{key.rfind(keydel)};
    const auto parent{pos == string::npos ? "" : key.substr(0, pos)};
    const auto leaf{pos == string::npos ? key : key.substr(pos + 1)};
    toml::table& root{parent.empty() ? tree : insert_table(parent)};
    if (root[leaf].type() != toml::node_type::none) {
        throw invalid_argument{"incorrect type for node '" + key + "'"};
    }
    root.emplace(leaf, toml::value<T>{});
}


toml::table& TomlConfig::insert_table(const string& key) {
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
