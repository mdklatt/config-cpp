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
     * Store TOML config data.
     *
     * Keys are hierarchical and specify a complete path to their target
     * value using dotted components, *e.g.* "table.nested.value".
     */
    class TomlConfig {
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
         * Load config data from an input stream.
         *
         * @param stream TOML data stream
         * @param root place data at this root
         */
        void load(std::istream& stream, const std::string& root="");

        /**
         * Load config data from a file path.
         *
         * @param path TOML file path
         * @param root place data at this root
         */
        void load(const std::filesystem::path& path, const std::string& root="");

        /**
         * Writeable access to a value.
         *
         * A new value node will be created if it does not exist, including all
         * parent nodes as necessary. An existing value must already have the
         * correct type or a `std::invalid_argument` exception will be thrown.
         *
         * @tparam T value type (long long, double, bool, or std::string)
         * @param key hierarchical key
         * @return value reference
         */
        template<typename T>
        T& at(const std::string& key);

        /**
         * Read-only access to a value.
         *
         * A `std::invalid_argument` exception will be thrown if the target is
         * not an existing value of the correct type.
         *
         * @tparam T value type (long long, double, bool, or std::string)
         * @param key hierarchical key
         */
        template<typename T>
        const T& at(const std::string& key) const;

        /**
         * Test if key exists.
         *
         * @param key hierarchical key
         * @return true if key exists
         */
        bool has_key(const std::string& key) const;
        
    private:
        static constexpr char keydel{'.'};
        toml::table tree;

        /**
         * Private implementation of at().
         *
         * @tparam T value type
         * @param key hierarchical key
         * @param type node type
         * @return value reference
         */
        template <typename T>
        T& at(const std::string& key, const toml::node_type& type);

        /**
         * Private implementation of at().
         *
         * @tparam T native C++ value type
         * @param key hierarchical key
         * @param type node type
         * @return value reference
         */
        template <typename T>
        const T& at(const std::string& key, const toml::node_type& type) const;

        /**
         * Insert a new value node at the given location.
         *
         * Parent nodes are created as necessary. A `std::invalid_argument`
         * exception is thrown if a parent node exists and is not a table or if
         * the target node exists and has the correct type.
         *
         * @param T native C++ value type
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

}  // namespace


#endif  // CONFIG_CONFIGURE_HPP
