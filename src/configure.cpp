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


void Config::load(istream& stream) {
    tree = toml::parse(stream);
}


void Config::load(const std::filesystem::path& path) {
    tree = toml::parse_file(path.string());
}


string& Config::operator[](const string& key) {
    auto node{tree.at_path(key)};
    if (not node.is_string()) {
        insert(key);
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


void Config::insert(const std::string& path) {
    static const char delimit{'.'};
    string parent;
    for (auto it{path.begin()}; it != path.end(); ++it) {
        // Create parent table nodes.
        if (*it == delimit) {
            parent.assign(path.begin(), it);
            if (tree.at_path(parent).type() == toml::node_type::none) {
                tree.emplace(parent, toml::table());
            }
            else if (not tree.at_path(parent).is_table()) {
                throw invalid_argument{"node '" + parent + "' is not a table"};
            }
            ++it;  // skip delimiter
        }
    }
    auto pos{path.rfind(delimit)};
    const auto leaf{pos == string::npos ? path : path.substr(++pos)};
    toml::table& root{path == leaf ? tree : tree.at_path(parent).ref<toml::table>()};
    if (root[leaf].type() != toml::node_type::none) {
        throw invalid_argument{"target node '" + path + "' is not a string"};
    }
    root.emplace(leaf, toml::value<string>{});
}