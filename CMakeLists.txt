cmake_minimum_required(VERSION 3.7)

project(mp3encoder)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

find_library(PTHREAD_LIBRARY pthread)

add_subdirectory(3rdparty/lame)

include_directories(3rdparty/lame/include)

file(GLOB_RECURSE SOURCES
    src/*.cpp
    src/*.h
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
)

target_link_libraries(${PROJECT_NAME}
    mp3lame
    ${PTHREAD_LIBRARY}
)
