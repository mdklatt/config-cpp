/**
 * Test suite for the configure module.
 *
 * Link all test files with the `gtest_main` library to create a command-line 
 * test runner.
 */
#include "config/configure.hpp"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <vector>

using namespace configure;
using std::invalid_argument;
using std::ifstream;
using std::ofstream;
using std::string;
using std::vector;
using testing::Test;


/**
 * Test fixture for the configure test suite.
 *
 * This is used to group tests and provide common set-up and tear-down code.
 * A new test fixture is created for each test to prevent any side effects
 * between tests. Member variables and methods are injected into each test that
 * uses this fixture.
 *
 * This will be parametrized over all logging levels.
 */
class ConfigTest: public Test {
protected:
    const string path{"tests/unit/assets/config.toml"};
    const vector<string> keys = {"key1", "key2"};
    const vector<string> values = {"value1", "value2"};
};


/**
 * Test the stream constructor.
 */
TEST_F(ConfigTest, ctor_stream) {
    ifstream stream{path};
    Config config{stream};
    for (size_t pos(0); pos != keys.size(); ++pos) {
        ASSERT_EQ(config[keys[pos]], values[pos]);
        ASSERT_EQ(config["section1." + keys[pos]], values[pos]);
        ASSERT_EQ(config["section1.table." + keys[pos]], values[pos]);
    }
}


/**
 * Test the file constructor.
 */
TEST_F(ConfigTest, ctor_path) {
    Config config{path};
    for (size_t pos(0); pos != keys.size(); ++ pos) {
        ASSERT_EQ(config[keys[pos]], values[pos]);
        ASSERT_EQ(config["section1." + keys[pos]], values[pos]);
        ASSERT_EQ(config["section1.table." + keys[pos]], values[pos]);
    }
}


/**
 * Test the load method for a stream.
 */
TEST_F(ConfigTest, load_stream) {
    ifstream stream{path};
    Config config;
    config.load(path);
    for (size_t pos(0); pos != keys.size(); ++pos) {
        ASSERT_EQ(config[keys[pos]], values[pos]);
        ASSERT_EQ(config["section1." + keys[pos]], values[pos]);
        ASSERT_EQ(config["section1.table." + keys[pos]], values[pos]);
    }
}


/**
 * Test the load method for a path.
 */
TEST_F(ConfigTest, load_path) {
    Config config;
    config.load(path);
    for (size_t pos{0}; pos != keys.size(); ++pos) {
        ASSERT_EQ(config[keys[pos]], values[pos]);
        ASSERT_EQ(config["section1." + keys[pos]], values[pos]);
        ASSERT_EQ(config["section1.table." + keys[pos]], values[pos]);
    }
}


/**
 * Test the load method for a path.
 */
TEST_F(ConfigTest, load_toml) {
    Config config;
    config.load(std::filesystem::path{path});
    for (size_t pos{0}; pos != keys.size(); ++pos) {
        ASSERT_EQ(config[keys[pos]], values[pos]);
        ASSERT_EQ(config["section1." + keys[pos]], values[pos]);
        ASSERT_EQ(config["section1.table." + keys[pos]], values[pos]);
    }
}


/**
 * Test value write.
 */
TEST_F(ConfigTest, write) {
    Config config;
    for (size_t pos(0); pos != keys.size(); ++pos) {
        config[keys[pos]] = values[pos];
        ASSERT_EQ(config[keys[pos]], values[pos]);
    }
}


/**
 * Test value write failure.
 */
TEST_F(ConfigTest, write_fail) {
    Config config{path};
    for (const auto key: {"section1", "section1.table"}) {
        // Cannot overwrite non-string nodes.
        ASSERT_THROW(config[key] = "abc", invalid_argument);
    }
}


/**
 * Test value read failure.
 */
TEST_F(ConfigTest, read_fail) {
    const Config config{path};
    ASSERT_THROW(config["none"], invalid_argument);
}
