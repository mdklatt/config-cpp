/**
 * Verify that the library is usable in an external CMake project by testing
 * that all headers are present and all modules are linkable.
 */
#include <cstdlib>
#include <string>
#include "config/toml.hpp"

using configure::TomlConfig;
using std::string;


/**
 * Execute the application.
 *
 * @return system exit status
 */
int main() {
    static const string path{"tests/integration/assets/config.toml"};
    static const string str{"STR"};
    const TomlConfig config{path, {{"str", str}}};
    return config.as_string("string") == str ? EXIT_SUCCESS : EXIT_FAILURE;
}
