cmake_minimum_required(VERSION 3.4.1)

project(cppserver)

set(CMAKE_CXX_STANDARD 11) 
set(CMAKE_CXX_STANDARD_REQUIRED True) 
set(CMAKE_CXX_FLAGS "-g")
set(CMAKE_CXX_FLAGS "-pthread")

include_directories(./include)

aux_source_directory(./src DIR_SRC)

add_executable(server server.cpp ${DIR_SRC})
add_executable(client client.cpp ./src/utilfunc.cpp)

add_executable(threadpool_test ./test/threadpool_test.cpp ${DIR_SRC})
