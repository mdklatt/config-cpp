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
         * Construct an object from a stream.
         *
         * @param stream TOML data stream
         */
        explicit JsonConfig(std::istream& stream);

        /**
         * Construct an object from a stream with parameter substitution
         *
         * @param stream TOML data stream
         * @param params parameter substitutions
         */
        JsonConfig(std::istream& stream, const Params& params);

        /**
         * Construct an object from a file.
         *
         * @param path TOML file path
         */
        explicit JsonConfig(const std::filesystem::path& path);

        /**
         * Construct an object from a stream with parameter substitution.
         *
         * @param path TOML file path
         * @param params parameter substitutions
         */
        JsonConfig(const std::filesystem::path& path, const Params &params);


        // TODO: Add support for boolean values.
    };

}  // namespace

#endif  // CONFIG_JSON_HPP
