cmake_minimum_required(VERSION 3.1)
project(min_ex CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(open62541 REQUIRED)

add_executable(main
        main.cpp
)

add_executable(can_server
        CANServer.cpp
)       

add_executable(location_server
        LocationServer.cpp
)       

add_executable(client
        client.cpp
        serial.cpp
        DiscoveryServer.cpp
        ServerConnector.cpp
        utils.cpp
)

target_link_libraries(main 
        open62541::open62541
        rt
)


target_link_libraries(can_server 
        open62541::open62541
        rt
        pthread
)

target_link_libraries(location_server
        open62541::open62541
        rt
        pthread
)

target_link_libraries(client 
        open62541::open62541
        pthread
        rt
)
