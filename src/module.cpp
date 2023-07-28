/**
 * Implementation of the sample library module.
 */
#include "config/module.hpp"

using config::SampleClass;


int SampleClass::add(int x) const {
    return config::add(num, x);
}


int config::add(int x, int y) {
    return x + y;
}
