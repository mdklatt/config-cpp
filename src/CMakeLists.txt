# Get dependencies.

include(FetchContent)

FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus
    GIT_TAG v3.3.0
)
FetchContent_MakeAvailable(tomlplusplus)


FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp
    GIT_TAG yaml-cpp-0.7.0
)
FetchContent_MakeAvailable(yaml-cpp)


# Define targets.

add_library(${name}
    configure.cpp
    yaml.cpp
)
add_library(config::config ALIAS config)

target_include_directories(${name}
PUBLIC
    # Interface paths are relative to the installation path, not the current
    # directory.
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(${name}
PUBLIC
    tomlplusplus::tomlplusplus
    yaml-cpp::yaml-cpp
)


# Generate package config files.

include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/config-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/config-config.cmake
INSTALL_DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/config
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/config-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)


# Define installation rules.

include(GNUInstallDirs)

install(TARGETS ${name}
    EXPORT config-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)


install(
FILES
    ${CMAKE_CURRENT_BINARY_DIR}/config-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/config-config-version.cmake
DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/config
)

# TODO: Need to export tomlplusplus
#install(EXPORT config-targets
#    FILE config-targets.cmake
#    NAMESPACE config::
#    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/config
#)


# Export targets. If an external project adds this project to its build tree
# via `add_subdirectory()`, then `find_package()` will work without
# requiring this project to be installed.

#export(EXPORT config-targets
#    FILE ${CMAKE_CURRENT_BINARY_DIR}/config-targets.cmake
#    NAMESPACE config::
#)
export(PACKAGE config)
