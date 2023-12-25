#include "config/json.hpp"

using std::istream;
using std::string;

using namespace configure;


JsonConfig::JsonConfig(istream& stream) {
    load(stream);
}


JsonConfig::JsonConfig(istream& stream, const Params& params) {
    load(stream, params);
}


JsonConfig::JsonConfig(const std::filesystem::path& path) {
    load(path);
}


JsonConfig::JsonConfig(const std::filesystem::path& path, const Params& params) {
    load(path, params);
}
