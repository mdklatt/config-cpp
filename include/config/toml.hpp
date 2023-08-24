/**
 * Handle TOML config files: <https://toml.io/en/>.
 *
 * @file
 */
#ifndef CONFIG_TOML_HPP
#define CONFIG_TOML_HPP

#include "config.hpp"
#include <toml++/toml.h>
#include <filesystem>
#include <istream>
#include <map>
#include <string>


namespace configure {

/**
 * Store TOML config data.
 *
 * Keys are hierarchical and specify a complete path to their target
 * value using dotted components, *e.g.* "table.nested.value".
 */
class TomlConfig: public Config {
public:
    /**
     * Default constructor.
     */
    TomlConfig() = default;

    /**
     * Construct a Config object from an input stream.
     *
     * @param stream TOML data stream
     */
    explicit TomlConfig(std::istream& stream);

    /**
     * Construct a Config object from a file.
     *
     * @param path TOML file path
     */
    explicit TomlConfig(const std::filesystem::path& path);

    /**
     * Access a boolean node.
     *
     * A new node will be created if it does not exist, including all
     * parent nodes as necessary. An existing value must already have the
     * expected type or an exception will be thrown.
     *
     * @param key hierarchical key
     * @return node value reference
     */
    bool& as_boolean(const std::string& key);

    /**
     * Get the value of a boolean node.
     *
     * An exception will be thrown if a node of the expected type does not
     * already exist.
     *
     * @param key hierarchical key
     * @return node value reference
     */
    const bool& as_boolean(const std::string& key) const;

    /**
     * Get the value of a boolean node.
     *
     * If the node does not exist, the fallback value will be returned. An
     * exception will be thrown if the node exists but does not have the
     * expected type.
     *
     * @param key hierarchical key
     * @param fallback default value if node does not exist
     * @return node value or fallback
     */
    bool as_boolean(const std::string& key, const bool& fallback) const;

protected:
    /**
     * Load config data from an input stream.
     *
     * @param stream TOML data stream
     * @param root place data at this root
     */
    toml::table parse(std::istream& stream) override;

    /**
     * Load config data from a file path.
     *
     * @param path TOML file path
     * @param root place data at this root
     */
    toml::table parse(const std::filesystem::path& path) override;
};

}  // namespace


#endif  // CONFIG_TOML_HPP
