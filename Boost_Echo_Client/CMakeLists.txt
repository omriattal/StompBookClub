cmake_minimum_required(VERSION 3.15)

project(Boost_Echo_Client)

set(CMAKE_CXX_STANDARD 11)

find_package(Boost 1.57.0 COMPONENTS system filesystem thread REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

include_directories(include)
add_executable(StompClient
        include/ConnectionHandler.h
        src/ConnectionHandler.cpp
        src/StompClientRunner.cpp
        src/StompFrame.cpp
        include/StompFrame.h src/User.cpp include/User.h src/StompProtocol.cpp include/StompProtocol.h src/Book.cpp include/Book.h include/StompClientRunner.h src/Parser.cpp include/Parser.h src/StompClient.cpp)
target_link_libraries(StompClient ${Boost_LIBRARIES})