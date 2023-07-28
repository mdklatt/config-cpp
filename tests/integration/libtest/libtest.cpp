/**
 * Verify that the library is usable in an external CMake project by testing
 * that all headers are present and all modules are linkable.
 */
#include "config/module.hpp"


/**
 * Execute the application.
 *
 * @return system exit status
 */
int main() {
    return config::add(1, 2) == 3 ? 0 : 1;
}
