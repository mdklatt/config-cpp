/**
 * Implementation of the configure module.
 */
#include "config/configure.hpp"
#include <fstream>
#include <istream>
#include <iterator>
#include <list>
#include <stdexcept>


using std::getline;
using std::invalid_argument;
using std::istream;
using std::list;
using std::next;
using std::out_of_range;
using std::runtime_error;
using std::string;
using std::string_view;

using namespace configure;


Config::Config(istream& stream) {
    load(stream);
}


Config::Config(const std::filesystem::path& path) {
    load(path);
}


void Config::load(istream& stream, const string& root) {
    const auto table{toml::parse(stream)};
    if (root.empty()) {
        tree = table;
    }
    else {
        insert_table(root) = table;
    }
}


void Config::load(const std::filesystem::path& path, const string& root) {
    const auto table{toml::parse_file(path.string())};
    if (root.empty()) {
        tree = table;
    }
    else {
        insert_table(root) = table;
    }
}


string& Config::operator[](const string& key) {
    auto node{tree.at_path(key)};
    if (not node.is_string()) {
        insert_string(key);
        node = tree.at_path(key);  // need to reassign to new node
    }
    return node.ref<string>();
}
    

const string& Config::operator[](const string& key) const {
    const auto node{tree.at_path(key)};
    if (not node.is_string()) {
        throw invalid_argument{"node '" + key + "' is not a string"};
    }
    return node.ref<string>();
}


bool Config::has_key(const string& key) const {
    return tree.at_path(key).type() != toml::node_type::none;
}


void Config::insert_string(const std::string& key) {
    const auto pos{key.rfind(keydel)};
    const auto parent{pos == string::npos ? "" : key.substr(0, pos)};
    const auto leaf{pos == string::npos ? key : key.substr(pos + 1)};
    toml::table& root{parent.empty() ? tree : insert_table(parent)};
    if (root[leaf].type() != toml::node_type::none) {
        throw invalid_argument{"target node '" + key + "' is not a string"};
    }
    root.emplace(leaf, toml::value<string>{});
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