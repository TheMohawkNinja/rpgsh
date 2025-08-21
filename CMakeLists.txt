#Header
cmake_minimum_required(VERSION 3.10)
project(rpgsh VERSION 0.19.9)

#Set variables
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra -Wpedantic -g")
set(CXX_STD_FLAGS "-lstdc++fs -std=c++20")
set(libs "-lcolors -lconfig -lcurrency -ldefine -ldice -lfunctions -lscope -lvar")
set(CMAKE_CURRENT_SOURCE_DIR src/)
set(CMAKE_CURRENT_HEADERS_DIR headers/)

#Library objects
foreach(lib colors config currency dice define functions scope var)
	message("Adding ${lib} library")
	add_library(${lib} SHARED ${CMAKE_CURRENT_SOURCE_DIR}${lib}.cpp)
endforeach()

message("\n")

#Binaries
message("Adding main binary")
add_executable(rpgsh ${CMAKE_CURRENT_SOURCE_DIR}main.cpp)
target_link_libraries(rpgsh PUBLIC ${CMAKE_CURRENT_HEADERS_DIR})
target_include_directories(rpgsh PUBLIC "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}${CMAKE_CURRENT_SOURCE_DIR}")
foreach(bin autorun banner clear copy del edit eval find help history list load new print roll setname valueof version)
	message("Adding ${bin} binary")
	add_executable(_${bin} ${CMAKE_CURRENT_SOURCE_DIR}${bin}.cpp)#Use of underscore because "help" is reserved
	target_link_libraries(_${bin} PUBLIC ${CMAKE_CURRENT_HEADERS_DIR})
	target_include_directories(_${bin} PUBLIC "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}${CMAKE_CURRENT_SOURCE_DIR}")
endforeach()

#Done!
message("\nDone!")
