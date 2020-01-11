cmake_minimum_required(VERSION 3.15)

project(Boost_Echo_Client)

set(CMAKE_CXX_STANDARD 11)

find_package(Boost 1.57.0 COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

include_directories(include)
add_executable(Boost_Echo_client
        include/ConnectionHandler.h
        src/ConnectionHandler.cpp
        src/echoClient.cpp src/StompClient.cpp
        src/StompFrame.cpp
        include/StompFrame.h src/User.cpp include/User.h src/StompProtocol.cpp include/StompProtocol.h src/Book.cpp include/Book.h include/StompClient.h)
target_link_libraries(Boost_Echo_client ${Boost_LIBRARIES})