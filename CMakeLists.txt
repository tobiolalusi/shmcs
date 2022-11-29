cmake_minimum_required(VERSION 3.14)
project(shm-client-server LANGUAGES CXX)

# set compile flags
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_COVERAGE} ${CMAKE_CXX_FLAGS_DEBUG} -fprofile-instr-generate -fcoverage-mapping")

add_compile_options(-Wall -Wextra -pedantic -Wformat=2 -g)

# detect dependencies
find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)
find_package(fmt REQUIRED)

# compile protobuf messages
protobuf_generate_cpp(PROTO_SRC PROTO_HDR ${PROJECT_SOURCE_DIR}/lib/message/shm.proto)

add_library(shmcs
        include/shmcs/connection.hh
        include/shmcs/handler.hh
        include/shmcs/server.hh
        include/shmcs/types.hh
        lib/server.cc
        lib/connection.cc
        ${PROTO_SRC}
        ${PROTO_HDR})

target_include_directories(shmcs
        PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
        PUBLIC ${PROTOBUF_INCLUDE_DIR}
        PUBLIC ${PROJECT_SOURCE_DIR}/include
        PRIVATE ${PROJECT_SOURCE_DIR}/lib)

target_link_libraries(shmcs
        PUBLIC ${PROTOBUF_LIBRARY}
        PRIVATE fmt::fmt
        PRIVATE Threads::Threads)

# server executable
add_executable(server src/server.cc)
target_link_libraries(server shmcs)

# debug prints
if (NOT DEFINED DEBUG)
    set(DEBUG 0)
endif()
if (NOT DEFINED INFO)
    set(INFO DEBUG)
endif()
add_definitions(-DDEBUG=${DEBUG} -DINFO={INFO})
