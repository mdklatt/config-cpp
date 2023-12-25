#include "config/yaml.hpp"
#include <yaml-cpp/yaml.h>
#include <cstdlib>
#include <stdexcept>
#include <vector>


using std::invalid_argument;
using std::istream;
using std::string;
using std::vector;
using std::strtod;
using std::strtol;
using YAML::Load;
using YAML::LoadFile;

using namespace configure;


namespace {

class Number {
public:
    enum { integer, decimal, none } type;
    union { long long integer; double decimal; } value;

    explicit Number(const std::string& strval) {
        vector<char> buffer{strval.begin(), strval.end()};
        buffer.emplace_back('\0');
        char* fptr{buffer.data()};
        double f{strtod(buffer.data(), &fptr)};
        char* iptr{buffer.data()};
        long long i{strtol(buffer.data(), &iptr, 0)};
        if (*fptr != '\0' and *iptr != '\0') {
            type = none;
        }
        else if (*iptr == '\0') {
            type = integer;
            value.integer = i;
        }
        else {  // *fptr == '\0'
            type = decimal;
            value.decimal = f;
        }
    }

};

}


YamlConfig::YamlConfig(istream& stream) {
    load(stream);
}


YamlConfig::YamlConfig(istream& stream, const Params& params) {
    load(stream, params);
}


YamlConfig::YamlConfig(const std::filesystem::path& path) {
    load(path);
}


YamlConfig::YamlConfig(const std::filesystem::path& path, const Params& params) {
    load(path, params);
}


toml::table YamlConfig::parse(std::istream& stream) {
    const auto node{Load(stream)};
    toml::table table;
    insert(table, node);
    return table;
}


toml::table YamlConfig::parse(const std::filesystem::path& path) {
    const auto node{LoadFile(path)};
    toml::table table;
    insert(table, node);
    return table;
}


void YamlConfig::insert(toml::node& table, const YAML::Node& node) {
    for (const auto& item: node) {
        const auto key{item.first.Scalar()};
        const auto value{item.second};
        if (value.IsMap()) {
            auto result{table.as_table()->emplace(key, toml::table())};
            if (not result.second) {
                throw std::runtime_error{"could not insert table at "  + key};
            }
            insert(result.first->second, value);
        }
        else if (value.IsScalar()) {
            const auto strval{value.Scalar()};
            const Number numval{strval};
            switch (numval.type) {
                case Number::integer:
                    table.as_table()->emplace(key, numval.value.integer);
                    break;
                case Number::decimal:
                    table.as_table()->emplace(key, numval.value.decimal);
                    break;
                case Number::none:
                    table.as_table()->emplace(key, strval);
                    break;
            }
        }
        else {
            // TODO: Arrays.
        }
    }
}
