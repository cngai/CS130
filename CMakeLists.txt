cmake_minimum_required(VERSION 3.10.0)
project(monkeydb)

# Make sure developers do not run cmake in the main project directory, to keep
# build artifacts from becoming clutter
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed.
    Please make a new directory (called a build directory) and run CMake from there.
    You may need to remove CMakeCache.txt." )
endif()

# Turn on debug builds if we are building in a devel shell
if (CMAKE_BUILD_TYPE STREQUAL "" AND "$ENV{DEVEL_SHELL}" STREQUAL "1")
    message(STATUS "Setting debug build type by default in devel shell")
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Output binaries to a sub directory "bin"
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable cmake testing
include(CTest)
enable_testing()

# Enable GoogleTest
include(GoogleTest)
add_subdirectory(/usr/src/googletest googletest)

# Enable Boost
# Use static libraries so binaries can be deployed without a full boost install
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.50 REQUIRED COMPONENTS system filesystem log regex)
message(STATUS "Boost version: ${Boost_VERSION}")

include_directories(include)

add_library(logger src/logger.cc)
add_library(config_parser src/config_parser.cc)
add_library(op_server src/server.cc src/session.cc src/request/handler_dispatcher.cc)
add_library(session src/session.cc src/server.cc src/request/handler_dispatcher.cc)
add_library(request_parser src/request/request_parser.cc)
add_library(response src/request/response.cc)
add_library(dispatch src/request/handler_dispatcher.cc src/config_parser.cc)
add_library(request_handler src/request/echo_handle.cc src/request/request_parser.cc src/request/static_file_handle.cc src/request/fof_handle.cc src/request/status_handle.cc src/request/handler_dispatcher.cc src/request/proxy_handle.cc src/request/health_handle.cc src/request/covid_handle.cc)


target_link_libraries(logger Boost::system Boost::filesystem Boost::regex Boost::log_setup Boost::log)
target_link_libraries(op_server logger Boost::system Boost::filesystem Boost::regex Boost::log_setup Boost::log)
target_link_libraries(session logger Boost::system Boost::filesystem Boost::regex Boost::log_setup Boost::log)

add_executable(server src/server_main.cc)
target_link_libraries(server op_server session config_parser request_parser response request_handler logger dispatch Boost::system Boost::filesystem Boost::log_setup Boost::log)

# For config parser unit test
add_executable(config_parser_test tests/config_parser_test.cc)
target_link_libraries(config_parser_test config_parser op_server session gtest_main)

# # For request parser unit test
add_executable(request_parser_test tests/request_parser_test.cc)
target_link_libraries(request_parser_test request_parser gtest_main)

# # For session unit test
add_executable(session_test tests/session_test.cc)
target_link_libraries(session_test session request_parser request_handler response gtest_main logger dispatch Boost::system Boost::filesystem Boost::log_setup Boost::log)

# For server unit test
add_executable(server_test tests/server_test.cc)
target_link_libraries(server_test op_server request_handler response gtest_main dispatch logger Boost::system Boost::filesystem Boost::log_setup Boost::log)

# For server response test
add_executable(server_reply_test tests/server_reply_test.cc)
target_link_libraries(server_reply_test response gtest_main Boost::system Boost::filesystem Boost::regex Boost::log_setup Boost::log)

# For Logger test
add_executable(logger_test tests/logger_test.cc)
target_link_libraries(logger_test logger gtest_main Boost::log session op_server request_parser request_handler config_parser response)

# For all handerl tests
add_executable(handle_reply_test tests/handle_reply_test.cc)
# May need to add more as we write the actual  unit tests
target_link_libraries(handle_reply_test  gtest_main session request_parser request_handler config_parser response)

gtest_discover_tests(config_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(request_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(session_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(server_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(server_reply_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(logger_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(handle_reply_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

add_test(NAME integration_test COMMAND ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/tests/server_test.sh)

include(cmake/CodeCoverageReportConfig.cmake)
generate_coverage_report(TARGETS config_parser op_server session request_parser response request_handler TESTS config_parser_test request_parser_test session_test server_test server_reply_test logger_test handle_reply_test)
