/**
 * Global application configuration.
 *
 * This module defines a global configuration object that other modules can 
 * use to store application-wide configuration values.
 *
 * @file
 */
#ifndef CONFIG_CONFIG_HPP
#define CONFIG_CONFIG_HPP

#include <toml++/toml.h>
#include <cstdint>
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
    using Params = std::map<std::string, std::string>;

    /**
     * Load config data from an input stream.
     *
     * @param stream input stream
     * @param root place data at this root
     * @param params parameter substitutions
     */
    void load(std::istream& stream, const std::string& root, const Params& params);

    /** @overload */
    void load(std::istream& stream, const std::string& root="");

    /** @overload */
    void load(std::istream& stream, const Params& params);

    /**
     * Load config data from a file path.
     *
     * @param path input file path
     * @param root place data at this root
     * @param params parameter substitutions
     */
    void load(const std::filesystem::path& path, const std::string& root, const Params& params);

    /** overload */
    void load(const std::filesystem::path& path, const std::string& root="");

    /** overload */
    void load(const std::filesystem::path& path, const Params& params);

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
     * An exception will be thrown if a node of the expected type does not
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
     * expected type.
     *
     * @param key hierarchical key
     * @param fallback default value if node does not exist
     * @return node value or fallback
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
     * An exception will be thrown if a node of the expected type does not
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
     * expected type.
     *
     * @param key hierarchical key
     * @param fallback default value if node does not exist
     * @return node value or fallback
     */
    int64_t as_integer(const std::string& key, const int64_t& fallback) const;

    /**
     * Access a string node.
     *
     * A new node will be created if it does not exist, including all
     * parent nodes as necessary. An existing value must already have the
     * expected type or an exception will be thrown.
     *
     * @param key hierarchical key
     * @return node value reference
     */
    std::string& as_string(const std::string& key);

    /**
     * Get the value of a string node.
     *
     * An exception will be thrown if a node of the expected type does not
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
     * expected type.
     *
     * @param key hierarchical key
     * @param fallback default value if node does not exist
     * @return node value or fallback
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
     * Default constructor.
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
     * Access a node.
     *
     * A new node will be created if it does not exist, including all
     * parent nodes as necessary. An existing value must already have the
     * correct type or an exception will be thrown.
     *
     * @tparam T value type
     * @param key hierarchical key
     * @param type node type
     * @return node value reference
     */
    template <typename T>
    T& at(const std::string& key, const toml::node_type& type);

    /**
     * Get the value of a node.
     *
     * An exception will be thrown if a node of the expected type does not
     * already exist.
     *
     * @tparam T value type
     * @param key hierarchical key
     * @param type node type
     * @return node value reference
     */
    template <typename T>
    const T& at(const std::string& key, const toml::node_type& type) const;

    /**
     * Get the value of a node.
     *
     * If the node does not exist, the fallback value will be returned. An
     * exception will be thrown if the node exists but does not have the
     * expected type.
     *
     * @tparam T value type
     * @param key hierarchical key
     * @param type node type
     * @param fallback default value if node does not exist
     * @return node value or fallback
     */
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
     * the target node exists and has the expected type.
     *
     * @param T value type
     * @param key hierarchical key
     */
    template <typename T>
    void insert_value(const std::string& key);

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

}  // namespace


#endif  // CONFIG_CONFIG_HPP
