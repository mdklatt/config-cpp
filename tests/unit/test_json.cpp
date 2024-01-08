/**
 * Test suite for the config library.
 *
 * Link all test files with the `gtest_main` library to create a command-line 
 * test runner.
 */
#include "config/json.hpp"
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
 * Test fixture for the JsonConfig class test suite.
 *
 * This is used to group tests and provide common set-up and tear-down code.
 * A new test fixture is created for each test to prevent any side effects
 * between tests. Member variables and methods are injected into each test that
 * uses this fixture.
 */
class JsonConfigTest: public Test {
protected:
    const string path{"tests/unit/assets/config.json"};
    const vector<string> prefixes{"", "section.", "section.table."};
    const vector<string> keys{"key1", "key2"};
    const vector<string> values{"value1", "value2"};
    const JsonConfig::Params params{{"str", "string"}, {"int", "123"}};
};


/**
 * Test the stream constructor.
 */
TEST_F(JsonConfigTest, ctor_stream) {
    ifstream stream{path};
    JsonConfig config{stream, params};
    for (const auto& prefix: prefixes) {
        const auto key = [&prefix](const string& name) {
            return prefix + name;
        };
        EXPECT_EQ(config.as_integer(key("int")), 123);
        EXPECT_EQ(config.as_integer(key("intx"), 123), 123);
        EXPECT_EQ(config.as_real(key("float")), 1.23);
        EXPECT_EQ(config.as_real(key("floatx"), 1.23), 1.23);
        EXPECT_EQ(config.as_string(key("string")), "string");
        EXPECT_EQ(config.as_string(key("stringx"), "string"), "string");
    }
}


/**
 * Test the load() method for a stream.
 */
TEST_F(JsonConfigTest, load_stream) {
    ifstream stream{path};
    JsonConfig config;
    config.load(stream, params);
    for (const auto& prefix: prefixes) {
        const auto key = [&prefix](const string& name) {
            return prefix + name;
        };
        EXPECT_EQ(config.as_integer(key("int")), 123);
        EXPECT_EQ(config.as_real(key("float")), 1.23);
        EXPECT_EQ(config.as_string(key("string")), "string");
    }
}


/**
 * Test the load() method for a stream with on optional root.
 */
TEST_F(JsonConfigTest, load_stream_root) {
    static const string root{"sub"};
    ifstream stream{path};
    JsonConfig config;
    config.load(stream, root, params);
    for (const auto& prefix: prefixes) {
        const auto key = [&prefix](const string& name) {
            return root + "." + prefix + name;
        };
        EXPECT_EQ(config.as_integer(key("int")), 123);
        EXPECT_EQ(config.as_real(key("float")), 1.23);
        EXPECT_EQ(config.as_string(key("string")), "string");
    }
}


/**
 * Type-parametrized fixture for JsonConfig class path tests.
 *
 * This is used to group tests and provide common set-up and tear-down code.
 * A new test fixture is created for each test to prevent any side effects
 * between tests. Member variables and methods are injected into each test that
 * uses this fixture.
 */
template <typename T>
class JsonConfigPathTest: public JsonConfigTest {
protected:
    const JsonConfig::Params params{{"str", "string"}, {"int", "123"}};
};

using PathTypes = ::testing::Types<string, std::filesystem::path>;
TYPED_TEST_SUITE(JsonConfigPathTest, PathTypes);


/**
 * Test the path constructor.
 */
TYPED_TEST(JsonConfigPathTest, ctor_path) {
    const TypeParam path{this->path};
    JsonConfig config{path, this->params};
    for (const auto& prefix: this->prefixes) {
        const auto key = [&prefix](const string& name) {
            return prefix + name;
        };
        EXPECT_EQ(config.as_integer(key("int")), 123);
        EXPECT_EQ(config.as_real(key("float")), 1.23);
        EXPECT_EQ(config.as_string(key("string")), "string");
    }
}


/**
 * Test the load() method for a path.
 */
TYPED_TEST(JsonConfigPathTest, load_path) {
    const TypeParam path{this->path};
    JsonConfig config;
    config.load(path, this->params);
    for (const auto& prefix: this->prefixes) {
        const auto key = [&prefix](const string& name) {
            return prefix + name;
        };
        EXPECT_EQ(config.as_integer(key("int")), 123);
        EXPECT_EQ(config.as_real(key("float")), 1.23);
        EXPECT_EQ(config.as_string(key("string")), "string");
    }
}


/**
 * Test the load() method for a path with on optional root.
 */
TYPED_TEST(JsonConfigPathTest, load_path_root) {
    static const string root{"sub"};
    const TypeParam path{this->path};
    JsonConfig config;
    config.load(path, root, this->params);
    for (const auto& prefix: this->prefixes) {
        const auto key = [&prefix](const string& name) {
            return root + "." + prefix + name;
        };
        EXPECT_EQ(config.as_integer(key("int")), 123);
        EXPECT_EQ(config.as_real(key("float")), 1.23);
        EXPECT_EQ(config.as_string(key("string")), "string");
    }
}


/**
 * Test the load() method for multiple calls.
 */
TEST_F(JsonConfigTest, load_multi) {
    JsonConfig config;
    config.load(path, params);
    config.load(path, "sub", params);
    for (const string& root: {"", "sub."}) {
        for (const auto &prefix: prefixes) {
            const auto key = [&root, &prefix](const string& name) {
                return root + prefix + name;
            };
            EXPECT_EQ(config.as_integer(key("int")), 123);
            EXPECT_EQ(config.as_real(key("float")), 1.23);
            EXPECT_EQ(config.as_string(key("string")), "string");
        }
    }
}


/**
 * Test value write.
 */
TEST_F(JsonConfigTest, write) {
    JsonConfig config;
    for (size_t pos(0); pos != keys.size(); ++pos) {
        config.as_string(keys[pos]) = values[pos];
        EXPECT_EQ(config.as_string(keys[pos]), values[pos]);
    }
}


/**
 * Test value write failure.
 */
TEST_F(JsonConfigTest, write_fail) {
    JsonConfig config{path, params};
    for (const auto key: {"section", "section.table"}) {
        // Cannot overwrite non-string nodes.
        EXPECT_THROW(config.as_string(key) = "abc", invalid_argument);
    }
}


/**
 * Test value read failure.
 */
TEST_F(JsonConfigTest, read_fail) {
    const JsonConfig config{path, params};
    EXPECT_THROW(config.as_string("none"), invalid_argument);
}


/**
 * Test the has_key() method.
 */
TEST_F(JsonConfigTest, has_key) {
    JsonConfig config{path, params};
    EXPECT_TRUE(config.has_key("section.string"));
    EXPECT_FALSE(config.has_key("section.none"));
}

