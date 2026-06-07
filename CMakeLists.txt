cmake_minimum_required(VERSION 3.10)
project(lab06)

set(BANKING_VERSION_MAJOR 0)
set(BANKING_VERSION_MINOR 1)
set(BANKING_VERSION_PATCH 0)
set(BANKING_VERSION_TWEAK 0)
set(BANKING_VERSION "${BANKING_VERSION_MAJOR}.${BANKING_VERSION_MINOR}.${BANKING_VERSION_PATCH}.${BANKING_VERSION_TWEAK}")
set(BANKING_VERSION_STRING "v${BANKING_VERSION}")

option(BUILD_TESTS "Build tests" OFF)

add_subdirectory(banking)

if(BUILD_TESTS)
    enable_testing()
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    add_subdirectory(third-party/gtest)

    add_executable(check tests/test_account.cpp tests/test_transaction.cpp)
    target_link_libraries(check banking gtest_main)
    target_include_directories(check PRIVATE banking)

    add_test(NAME check COMMAND check)
endif()

include(CPackConfig.cmake)
