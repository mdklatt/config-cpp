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
     * Store application config data.
     */
    class Config {
    public:
        /**
         * Default constructor.
         */
        Config() = default;

        /**
         * Construct a Config object from an input stream.
         *
         * @param stream TOML data stream
         */
        explicit Config(std::istream& stream);

        /**
         * Construct a Config object from a file.
         *
         * @param path TOML file path
         */
        explicit Config(const std::filesystem::path& path);

        /**
         * Load config data from an input stream.
         *
         * @param stream TOML data stream
         */
        void load(std::istream& stream);

        /**
         * Load config data from a file path.
         *
         * @param path TOML file path
         */
        void load(const std::filesystem::path& path);

        /**
         * Writeable access to a string value.
         *
         * Keys are hierarchical and specify a complete path to their target
         * value using dotted components, *e.g.* "table.nested.value". A new
         * value node  will be created if it does not exist, including all
         * parent nodes as necessary. Any existing nodes must already have the
         * correct type or a `std::invalid_argument` exception will be thrown.
         *
         *
         * @param key hierarchical key
         * @return value reference
         */
        std::string& operator[](const std::string& key);

        /**
         * Read-only access to a string value.
         *
         * Keys are hierarchical and specify a complete path to their target
         * value using dotted components, *e.g.* "table.nested.value". A
         * `std::invalid_argument` exception will be thrown if the target is
         * not an existing string node.
         *
         * @param key hierarchical key
         */
        const std::string& operator[](const std::string& path) const;
        
    private:
        toml::table tree;

        /**
         * Insert a new string node at the given location.
         *
         * Parent nodes are created as necessary. A `std::invalid_argument`
         * exception is thrown if a parent node exists and is not a tabel or if
         * the target node exists and is not a string.
         *
         * @param path full path to target node, *e.g.* "root.nested.value"
         */
        void insert(const std::string& path);
    };

}  // namespace


#endif  // CONFIG_CONFIGURE_HPP
