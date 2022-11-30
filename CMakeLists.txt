cmake_minimum_required(VERSION 3.14)
project(shmcs LANGUAGES CXX)

# set compile flags
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_COVERAGE} ${CMAKE_CXX_FLAGS_DEBUG} -fprofile-instr-generate -fcoverage-mapping")

add_compile_options(-Wall -Wextra -pedantic -Wformat=2 -g)

set(LIBRARY_NAME shmcs)
set(SERVER_EXECUTABLE_NAME server)
set(CLIENT_EXECUTABLE_NAME client)

# detect dependencies
find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)
find_package(fmt REQUIRED)

# compile protobuf messages
protobuf_generate_cpp(PROTO_SRC PROTO_HDR ${PROJECT_SOURCE_DIR}/lib/message/shm.proto)

add_library(${LIBRARY_NAME}
        include/shmcs/handler/handler.hh
        include/shmcs/handler/server.hh
        include/shmcs/network/connection.hh
        include/shmcs/network/server.hh
        include/shmcs/hashtable.hh
        include/shmcs/linkedlist.hh
        include/shmcs/spmc.hh
        include/shmcs/types.hh
        lib/handler/server.cc
        lib/network/server.cc
        lib/network/connection.cc
        ${PROTO_SRC}
        ${PROTO_HDR})

target_include_directories(${LIBRARY_NAME}
        PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
        PUBLIC ${PROTOBUF_INCLUDE_DIR}
        PUBLIC ${PROJECT_SOURCE_DIR}/include
        PRIVATE ${PROJECT_SOURCE_DIR}/lib)

target_link_libraries(shmcs
        PUBLIC ${PROTOBUF_LIBRARY}
        PRIVATE fmt::fmt
        PRIVATE Threads::Threads)

# server executable
add_executable(${SERVER_EXECUTABLE_NAME} src/server.cc)
target_link_libraries(${SERVER_EXECUTABLE_NAME} ${LIBRARY_NAME})

# testing
set(UNIT_TESTS_EXECUTABLE_NAME unit-tests)
set(UNIT_TESTS_SOURCES
        tests/unit/unit-tests.cc
        tests/unit/hashtable-test.cc
        tests/unit/linkedlist-test.cc)

find_package(doctest REQUIRED)
include(doctest)

add_executable(${UNIT_TESTS_EXECUTABLE_NAME} ${UNIT_TESTS_SOURCES})
target_link_libraries(${UNIT_TESTS_EXECUTABLE_NAME} PRIVATE doctest::doctest)

# debug prints
if (NOT DEFINED DEBUG)
    set(DEBUG 0)
endif()
if (NOT DEFINED INFO)
    set(INFO DEBUG)
endif()
add_definitions(-DDEBUG=${DEBUG} -DINFO={INFO})
