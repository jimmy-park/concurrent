cmake_minimum_required(VERSION 3.23)

project(
    concurrent
    VERSION 0.4.0
    DESCRIPTION "C++20 concurrent wrapper class"
    HOMEPAGE_URL "https://github.com/jimmy-park/concurrent"
    LANGUAGES CXX
)

# Custom options
option(CONCURRENT_COMPILE "Build as a static/shared library" OFF)
option(CONCURRENT_INSTALL "Install headers and CMake targets" OFF)

include(cmake/CompileOptions.cmake)

if(CONCURRENT_COMPILE)
    set(SCOPE PUBLIC)
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/concurrent.cpp "#include <concurrent.hpp>")
    add_library(concurrent ${CMAKE_CURRENT_BINARY_DIR}/concurrent.cpp)
else()
    set(SCOPE INTERFACE)
    add_library(concurrent INTERFACE)
endif()

add_library(concurrent::concurrent ALIAS concurrent)
target_compile_features(concurrent ${SCOPE} cxx_std_20)
target_sources(concurrent ${SCOPE}
    FILE_SET HEADERS
    BASE_DIRS include
    FILES include/concurrent.hpp
)

if(CONCURRENT_INSTALL)
    include(CMakePackageConfigHelpers)
    include(GNUInstallDirs)

    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/concurrent-config-version.cmake
        COMPATIBILITY SameMajorVersion
    )
    install(
        TARGETS concurrent
        EXPORT concurrentTargets
        FILE_SET HEADERS
    )
    install(
        EXPORT concurrentTargets
        NAMESPACE concurrent::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/concurrent
    )
    install(
        FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/concurrent-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/concurrent-config-version.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/concurrent
    )
endif()

if(PROJECT_IS_TOP_LEVEL AND NOT CMAKE_CROSSCOMPILING)
    include(CTest)
    add_subdirectory(test)
endif()
