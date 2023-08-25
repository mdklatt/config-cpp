#include "config/json.hpp"

using std::istream;
using std::string;

using namespace configure;


JsonConfig::JsonConfig(istream& stream, const string& root) {
    load(stream, root);
}


JsonConfig::JsonConfig(const std::filesystem::path& path, const string& root) {
    load(path, root);
}
