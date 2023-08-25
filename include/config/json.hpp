/**
 * Handle JSON config files: <https://www.json.org>.
 *
 * @file
 */
#ifndef CONFIG_JSON_HPP
#define CONFIG_JSON_HPP

#include "yaml.hpp"
#include <yaml-cpp/yaml.h>


namespace configure {
    /**
     * Store JSON config data.
     *
     * Keys are hierarchical and specify a complete path to their target
     * value using dotted components, *e.g.* "table.nested.value".
     */
    class JsonConfig: public YamlConfig {
    public:
        /**
         * Default constructor.
         */
        JsonConfig() = default;

        /**
         * Construct a JsonConfig object from an input stream.
         *
         * @param stream TOML data stream
         */
        explicit JsonConfig(std::istream& stream, const std::string& root="");

        /**
         * Construct a JsonConfig object from a file.
         *
         * @param path TOML file path
         */
        explicit JsonConfig(const std::filesystem::path& path, const std::string& root="");


        // TODO: Add support for boolean values.
    };

}  // namespace

#endif  // CONFIG_JSON_HPP
