/**
 * Test suite for the configure module.
 *
 * Link all test files with the `gtest_main` library to create a command-line 
 * test runner.
 */
#include "config/configure.hpp"
#include "config/yaml.hpp"
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
 * Test fixture for the TomlConfig class test suite.
 *
 * This is used to group tests and provide common set-up and tear-down code.
 * A new test fixture is created for each test to prevent any side effects
 * between tests. Member variables and methods are injected into each test that
 * uses this fixture.
 */
class TomlConfigTest: public Test {
protected:
    const string path{"tests/unit/assets/config.toml"};
    const vector<string> prefixes{"", "section." , "section.table."};
    const vector<string> keys{"key1", "key2"};
    const vector<string> values{"value1", "value2"};
};


/**
 * Test the stream constructor.
 */
TEST_F(TomlConfigTest, ctor_stream) {
    ifstream stream{path};
    TomlConfig config{stream};
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
TEST_F(TomlConfigTest, load_stream) {
    ifstream stream{path};
    TomlConfig config;
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
TEST_F(TomlConfigTest, load_stream_root) {
    static const string root{"sub"};
    ifstream stream{path};
    TomlConfig config;
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
 * Type-parametrized fixture for TomlConfig class path tests.
 *
 * This is used to group tests and provide common set-up and tear-down code.
 * A new test fixture is created for each test to prevent any side effects
 * between tests. Member variables and methods are injected into each test that
 * uses this fixture.
 */
template <typename T>
class TomlConfigPathTest: public TomlConfigTest {};

using PathTypes = ::testing::Types<string, std::filesystem::path>;
TYPED_TEST_SUITE(TomlConfigPathTest, PathTypes);


/**
 * Test the path constructor.
 */
TYPED_TEST(TomlConfigPathTest, ctor_path) {
    const TypeParam path{this->path};
    TomlConfig config{path};
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
TYPED_TEST(TomlConfigPathTest, load_path) {
    const TypeParam path{this->path};
    TomlConfig config;
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
TYPED_TEST(TomlConfigPathTest, load_path_root) {
    static const string root{"sub"};
    const TypeParam path{this->path};
    TomlConfig config;
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
TEST_F(TomlConfigTest, load_multi) {
    TomlConfig config;
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
TEST_F(TomlConfigTest, write) {
    TomlConfig config;
    for (size_t pos(0); pos != keys.size(); ++pos) {
        config.at<string>(keys[pos]) = values[pos];
        ASSERT_EQ(config.at<string>(keys[pos]), values[pos]);
    }
}


/**
 * Test value write failure.
 */
TEST_F(TomlConfigTest, write_fail) {
    TomlConfig config{path};
    for (const auto key: {"section", "section.table"}) {
        // Cannot overwrite non-string nodes.
        ASSERT_THROW(config.at<string>(key) = "abc", invalid_argument);
    }
}


/**
 * Test value read failure.
 */
TEST_F(TomlConfigTest, read_fail) {
    const TomlConfig config{path};
    ASSERT_THROW(config.at<string>("none"), invalid_argument);
}


/**
 * Test the has_key() method.
 */
 TEST_F(TomlConfigTest, has_key) {
     TomlConfig config{path};
     ASSERT_TRUE(config.has_key("section.string"));
     ASSERT_FALSE(config.has_key("section.none"));
 }


/**
 * Test fixture for the TomlConfig class test suite.
 *
 * This is used to group tests and provide common set-up and tear-down code.
 * A new test fixture is created for each test to prevent any side effects
 * between tests. Member variables and methods are injected into each test that
 * uses this fixture.
 */
class YamlConfigTest: public Test {
protected:
    const string path{"tests/unit/assets/config.yaml"};
    const vector<string> prefixes{"", "section." , "section.table."};
    const vector<string> keys{"key1", "key2"};
    const vector<string> values{"value1", "value2"};
};


/**
 * Test the stream constructor.
 */
TEST_F(YamlConfigTest, ctor_stream) {
    ifstream stream{path};
    YamlConfig config{stream};
    for (const auto& prefix: prefixes) {
        const auto key = [&prefix](const string& name) {
            return prefix + name;
        };
        ASSERT_EQ(config.at<string>(key("bool")), "true");
        ASSERT_EQ(config.at<double>(key("float")), 1.23);
        ASSERT_EQ(config.at<long long>(key("int")), 123);
        ASSERT_EQ(config.at<string>(key("string")), "string");
    }
}


/**
 * Test the load() method for a stream.
 */
TEST_F(YamlConfigTest, load_stream) {
    ifstream stream{path};
    YamlConfig config;
    config.load(stream);
    for (const auto& prefix: prefixes) {
        const auto key = [&prefix](const string& name) {
            return prefix + name;
        };
        ASSERT_EQ(config.at<string>(key("bool")), "true");
        ASSERT_EQ(config.at<double>(key("float")), 1.23);
        ASSERT_EQ(config.at<long long>(key("int")), 123);
        ASSERT_EQ(config.at<string>(key("string")), "string");
    }
}
