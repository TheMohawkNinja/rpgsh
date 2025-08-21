#Header
cmake_minimum_required(VERSION 3.10)
project(rpgsh VERSION 0.19.9)

#Set variables
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra -Wpedantic -g")
set(CXX_STD_FLAGS "-lstdc++fs -std=c++20")
set(libs "colors;config;currency;define;dice;functions;scope;var")
set(bins "autorun;banner;clear;copy;del;edit;eval;find;help;history;list;load;new;print;roll;setname;valueof;version")
list(LENGTH libs lib_length)
#message(${lib_length})
set(ENV{CMAKE_BUILD_PARALLEL_LEVEL} "${lib_length}")
message($ENV{CMAKE_BUILD_PARALLEL_LEVEL})
set(CMAKE_CURRENT_SOURCE_DIR src/)

#Library objects
foreach(lib ${libs})
	message("Adding ${lib} library")
	add_library(${lib} SHARED ${CMAKE_CURRENT_SOURCE_DIR}${lib}.cpp)
endforeach()

message("\n")

#Binaries
message("Adding main binary")
add_executable(rpgsh ${CMAKE_CURRENT_SOURCE_DIR}main.cpp)
target_link_libraries(rpgsh PUBLIC ${libs})
foreach(bin ${bins})
	message("Adding ${bin} binary")
	add_executable(rpgsh-${bin} ${CMAKE_CURRENT_SOURCE_DIR}${bin}.cpp)
	target_link_libraries(rpgsh-${bin} PUBLIC ${libs})
endforeach()

#Done!
message("\nDone!")
