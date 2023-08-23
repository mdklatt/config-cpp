/**
 * Global application configuration.
 *
 * This module defines a global configuration object that other modules can 
 * use to store application-wide configuration values.
 *
 * @file
 */
#ifndef CONFIG_CONFIGURE_HPP
#define CONFIG_CONFIGURE_HPP

#include <toml++/toml.h>
#include <filesystem>
#include <istream>
#include <map>
#include <string>


namespace configure {

/**
 * Abstract base class for configuration objects.
 */
class Config {
public:
    /**
     * Load config data from an input stream.
     *
     * @param stream input stream
     * @param root place data at this root
     */
    void load(std::istream& stream, const std::string& root="");

    /**
     * Load config data from a file path.
     *
     * @param path input file path
     * @param root place data at this root
     */
    void load(const std::filesystem::path& path, const std::string& root="");

    /**
     * Access a real number node.
     *
     * A new node will be created if it does not exist, including all
     * parent nodes as necessary. An existing value must already have the
     * correct type or an exception will be thrown.
     *
     * @param key hierarchical key
     * @return node value reference
     */
    double& as_real(const std::string& key);

    /**
     * Get the value of a real number node.
     *
     * An exception will be thrown if a node of the correct type does not
     * already exist.
     *
     * @param key hierarchical key
     * @return node value reference
     */
    const double& as_real(const std::string& key) const;

    /**
     * Get the value of a real number node.
     *
     * If the node does not exist, the fallback value will be returned. An
     * exception will be thrown if the node exists but does not have the
     * correct type.
     *
     * @param key hierarchical key
     * @return node value reference
     */
    double as_real(const std::string& key, const double& fallback) const;

    /**
     * Access an integer node.
     *
     * A new node will be created if it does not exist, including all
     * parent nodes as necessary. An existing value must already have the
     * correct type or an exception will be thrown.
     *
     * @param key hierarchical key
     * @return node value reference
     */
    int64_t& as_integer(const std::string& key);

    /**
     * Get the value of an integer node.
     *
     * An exception will be thrown if a node of the correct type does not
     * already exist.
     *
     * @param key hierarchical key
     * @return node value reference
     */
    const int64_t& as_integer(const std::string& key) const;

    /**
     * Get the value of an integer node.
     *
     * If the node does not exist, the fallback value will be returned. An
     * exception will be thrown if the node exists but does not have the
     * correct type.
     *
     * @param key hierarchical key
     * @return node value reference
     */
    int64_t as_integer(const std::string& key, const int64_t& fallback) const;

    /**
     * Access a string node.
     *
     * A new node will be created if it does not exist, including all
     * parent nodes as necessary. An existing value must already have the
     * correct type or an exception will be thrown.
     *
     * @param key hierarchical key
     * @return node value reference
     */
    std::string& as_string(const std::string& key);

    /**
     * Get the value of a string node.
     *
     * An exception will be thrown if a node of the correct type does not
     * already exist.
     *
     * @param key hierarchical key
     * @return node value reference
     */
    const std::string& as_string(const std::string& key) const;

    /**
     * Get the value of a string node.
     *
     * If the node does not exist, the fallback value will be returned. An
     * exception will be thrown if the node exists but does not have the
     * correct type.
     *
     * @param key hierarchical key
     * @return node value reference
     */
    std::string as_string(const std::string& key, const std::string& fallback) const;

    /**
     * Test if key exists.
     *
     * @param key hierarchical key
     * @return true if key exists
     */
    bool has_key(const std::string& key) const;

protected:
    /**
     * 
     */
    Config() = default;

    /**
     * Parse input from a stream.
     *
     * @param stream input stream
     * @return TOML data structure
     */
    virtual toml::table parse(std::istream& stream) = 0;

    /**
     * Parse input from a file.
     *
     * @param path input file path
     * @return TOML data structure
     */
    virtual toml::table parse(const std::filesystem::path& path) = 0;

    /**
     *
     *
     * @tparam T value type
     * @param key hierarchical key
     * @param type node type
     * @param insert true to insert node if it does not exist
     * @return value reference
     */
    template <typename T>
    T& at(const std::string& key, const toml::node_type& type);

    /**
     *
     *
     * @tparam T native C++ value type
     * @param key hierarchical key
     * @param type node type
     * @return value reference
     */
    template <typename T>
    const T& at(const std::string& key, const toml::node_type& type) const;

    template <typename T>
    T at(const std::string& key, const toml::node_type& type, const T& fallback) const;

private:
    static constexpr char keydel{'.'};
    toml::table tree;

    /**
     * Load data from a data structure.
     *
     * @param table TOML data structure
     * @param root place data at this root
     */
    void load(const toml::table&& table, const std::string& root);

    /**
     * Insert a new value node at the given location.
     *
     * Parent nodes are created as necessary. A `std::invalid_argument`
     * exception is thrown if a parent node exists and is not a table or if
     * the target node exists and has the correct type.
     *
     * @param T value type
     * @param key hierarchical key
     */
    template <typename T>
    void insert(const std::string& key);

    /**
     * Insert a new table node at the given location.
     *
     * Parent nodes are created as necessary. A `std::invalid_argument`
     * exception is thrown if a parent node exists and is not a table or if
     * the target node exists and is not a table.
     *
     * @param key hierarchical key
     */
    toml::table& insert_table(const std::string& key);
};


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
     * correct type or an exception will be thrown.
     *
     * @param key hierarchical key
     * @return node value reference
     */
    bool& as_boolean(const std::string &key);

    /**
     * Get the value of a boolean node.
     *
     * An exception will be thrown if a node of the correct type does not
     * already exist.
     *
     * @param key hierarchical key
     * @return node value reference
     */
    const bool& as_boolean(const std::string &key) const;

    /**
     * Get the value of a boolean node.
     *
     * If the node does not exist, the fallback value will be returned. An
     * exception will be thrown if the node exists but does not have the
     * correct type.
     *
     * @param key hierarchical key
     * @return node value reference
     */
    bool as_boolean(const std::string &key, const bool& fallback) const;

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


#endif  // CONFIG_CONFIGURE_HPP
