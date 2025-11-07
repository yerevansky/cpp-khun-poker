cmake_minimum_required(VERSION 3.14)

project(cpp-khun-poker LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

file(GLOB CPP_SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

file(GLOB CPP_HEADERS ${CMAKE_SOURCE_DIR}/include/*.h)

add_executable(cpp-khun-poke ${CPP_SOURCES} ${CPP_HEADERS})

target_include_directories(cpp-khun-poke PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(WIN32)
    source_group("Source Files" FILES ${CPP_SOURCES})
    source_group("Headers Files" FILES ${CPP_HEADERS})
endif()
