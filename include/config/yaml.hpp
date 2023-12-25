/**
 * Handle YAML config files: <https://yaml.org>.
 *
 * @file
 */
#ifndef CONFIG_YAML_HPP
#define CONFIG_YAML_HPP

#include "config.hpp"
#include <toml++/toml.h>
#include <yaml-cpp/yaml.h>


namespace configure {
    /**
     * Store YAML config data.
     *
     * Keys are hierarchical and specify a complete path to their target
     * value using dotted components, *e.g.* "table.nested.value".
     */
    class YamlConfig : public Config {
    public:
        /**
         * Default constructor.
         */
        YamlConfig() = default;

        /**
         * Construct an object from a stream.
         *
         * @param stream TOML data stream
         */
        explicit YamlConfig(std::istream& stream);

        /**
         * Construct an object from a stream with parameter substitution.
         *
         * @param stream TOML data stream
         * @param params parameter substitutions
         */
        YamlConfig(std::istream& stream, const Params& params);


        explicit YamlConfig(const std::filesystem::path& path);
        YamlConfig(const std::filesystem::path& path, const Params& params);


    protected:
        /**
         * Parse input from a stream.
         *
         * @param stream input stream
         * @return TOML data structure
         */
        toml::table parse(std::istream& stream) override;

        /**
         * Parse input from a file.
         *
         * @param path input file path
         * @return TOML data structure
         */
        toml::table parse(const std::filesystem::path& path) override;

    private:
        /**
         *
         * @param root
         * @param node sequence node to insert
         */
        void insert(toml::node& root, const YAML::Node& node);

    };

}  // namespace

#endif  // CONFIG_YAML_HPP
