/**
 * Test suite for the configure module.
 *
 * Link all test files with the `gtest_main` library to create a command-line 
 * test runner.
 */
#include "config/configure.hpp"
#include <gtest/gtest.h>
#include <filesystem>
#include <istream>
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
 * Test fixture for the Config class test suite.
 *
 * This is used to group tests and provide common set-up and tear-down code.
 * A new test fixture is created for each test to prevent any side effects
 * between tests. Member variables and methods are injected into each test that
 * uses this fixture.
 */
class ConfigTest: public Test {
protected:
    const string path{"tests/unit/assets/config.toml"};
    const vector<string> prefixes{"", "section." , "section.table."};
    const vector<string> keys{"key1", "key2"};
    const vector<string> values{"value1", "value2"};
};


/**
 * Test the stream constructor.
 */
TEST_F(ConfigTest, ctor_stream) {
    ifstream stream{path};
    Config config{stream};
    for (const auto& prefix: prefixes) {
        const auto key = [&prefix](const string& name) {
            return prefix + name;
        };
        ASSERT_EQ(config.at<bool>(key("bool")), true);
        ASSERT_EQ(config.at<long long>(key("int")), 123);
        ASSERT_EQ(config.at<double>(key("float")), 1.23);
        ASSERT_EQ(config.at<string>(key("string")), "string");
    }
}


/**
 * Test the load() method for a stream.
 */
TEST_F(ConfigTest, load_stream) {
    ifstream stream{path};
    Config config;
    config.load(stream);
    for (const auto& prefix: prefixes) {
        const auto key = [&prefix](const string& name) {
            return prefix + name;
        };
        ASSERT_EQ(config.at<bool>(key("bool")), true);
        ASSERT_EQ(config.at<long long>(key("int")), 123);
        ASSERT_EQ(config.at<double>(key("float")), 1.23);
        ASSERT_EQ(config.at<string>(key("string")), "string");
    }
}


/**
 * Test the load() method for a stream with on optional root.
 */
TEST_F(ConfigTest, load_stream_root) {
    static const string root{"sub"};
    ifstream stream{path};
    Config config;
    config.load(stream, root);
    for (const auto& prefix: prefixes) {
        const auto key = [&prefix](const string& name) {
            return root + "." + prefix + name;
        };
        ASSERT_EQ(config.at<bool>(key("bool")), true);
        ASSERT_EQ(config.at<long long>(key("int")), 123);
        ASSERT_EQ(config.at<double>(key("float")), 1.23);
        ASSERT_EQ(config.at<string>(key("string")), "string");
    }
}


/**
 * Type-parametrized fixture Config class path tests.
 *
 * This is used to group tests and provide common set-up and tear-down code.
 * A new test fixture is created for each test to prevent any side effects
 * between tests. Member variables and methods are injected into each test that
 * uses this fixture.
 */
template <typename T>
class ConfigPathTest: public ConfigTest {};

using PathTypes = ::testing::Types<string, std::filesystem::path>;
TYPED_TEST_SUITE(ConfigPathTest, PathTypes);


/**
 * Test the path constructor.
 */
TYPED_TEST(ConfigPathTest, ctor_path) {
    const TypeParam path{this->path};
    Config config{path};
    for (const auto& prefix: this->prefixes) {
        const auto key = [&prefix](const string& name) {
            return prefix + name;
        };
        ASSERT_EQ(config.at<bool>(key("bool")), true);
        ASSERT_EQ(config.at<long long>(key("int")), 123);
        ASSERT_EQ(config.at<double>(key("float")), 1.23);
        ASSERT_EQ(config.at<string>(key("string")), "string");
    }
}


/**
 * Test the load() method for a path.
 */
TYPED_TEST(ConfigPathTest, load_path) {
    const TypeParam path{this->path};
    Config config;
    config.load(path);
    for (const auto& prefix: this->prefixes) {
        const auto key = [&prefix](const string& name) {
            return prefix + name;
        };
        ASSERT_EQ(config.at<bool>(key("bool")), true);
        ASSERT_EQ(config.at<long long>(key("int")), 123);
        ASSERT_EQ(config.at<double>(key("float")), 1.23);
        ASSERT_EQ(config.at<string>(key("string")), "string");
    }
}


/**
 * Test the load() method for a path with on optional root.
 */
TYPED_TEST(ConfigPathTest, load_path_root) {
    static const string root{"sub"};
    const TypeParam path{this->path};
    Config config;
    config.load(path, root);
    for (const auto& prefix: this->prefixes) {
        const auto key = [&prefix](const string& name) {
            return root + "." + prefix + name;
        };
        ASSERT_EQ(config.at<bool>(key("bool")), true);
        ASSERT_EQ(config.at<long long>(key("int")), 123);
        ASSERT_EQ(config.at<double>(key("float")), 1.23);
        ASSERT_EQ(config.at<string>(key("string")), "string");
    }
}


/**
 * Test the load() method for multiple calls.
 */
TEST_F(ConfigTest, load_multi) {
    Config config;
    config.load(path);
    config.load(path, "sub");
    for (const string& root: {"", "sub."}) {
        for (const auto &prefix: prefixes) {
            const auto key = [&root, &prefix](const string& name) {
                return root + prefix + name;
            };
            ASSERT_EQ(config.at<bool>(key("bool")), true);
            ASSERT_EQ(config.at<long long>(key("int")), 123);
            ASSERT_EQ(config.at<double>(key("float")), 1.23);
            ASSERT_EQ(config.at<string>(key("string")), "string");
        }
    }
}


/**
 * Test value write.
 */
TEST_F(ConfigTest, write) {
    Config config;
    for (size_t pos(0); pos != keys.size(); ++pos) {
        config.at<string>(keys[pos]) = values[pos];
        ASSERT_EQ(config.at<string>(keys[pos]), values[pos]);
    }
}


/**
 * Test value write failure.
 */
TEST_F(ConfigTest, write_fail) {
    Config config{path};
    for (const auto key: {"section", "section.table"}) {
        // Cannot overwrite non-string nodes.
        ASSERT_THROW(config.at<string>(key) = "abc", invalid_argument);
    }
}


/**
 * Test value read failure.
 */
TEST_F(ConfigTest, read_fail) {
    const Config config{path};
    ASSERT_THROW(config.at<string>("none"), invalid_argument);
}


/**
 * Test the has_key() method.
 */
 TEST_F(ConfigTest, has_key) {
     Config config{path};
     ASSERT_TRUE(config.has_key("section.string"));
     ASSERT_FALSE(config.has_key("section.none"));
 }