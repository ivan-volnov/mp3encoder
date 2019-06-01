cmake_minimum_required(VERSION 3.7)

project(mp3lame)

add_definitions(-DHAVE_CONFIG_H)

include_directories(include)
include_directories(libmp3lame)

file(GLOB_RECURSE LAME_SOURCES
    libmp3lame/*.c
    libmp3lame/*.h
)

add_library(mp3lame STATIC ${LAME_SOURCES})
