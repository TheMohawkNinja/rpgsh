#Header
cmake_minimum_required(VERSION 3.10)
project(rpgsh)

#Set variables
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra -Wpedantic -g")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(prefix "rpgsh-")
set(libs "colors;configuration;currency;define;dice;functions;scope;var")
set(bins "autorun;banner;clear;config;copy;del;edit;eval;find;help;history;import;list;load;new;print;roll;setname;valueof;version")
set(SRC_DIR src/)
set(HEADERS_DIR headers/)
set(RPGSH_INSTALL_DIR_MACRO "#define RPGSH_INSTALL_DIR		")

#Insert the dynamically-generated install path into define.h
file(READ ${HEADERS_DIR}define.h TEXT)
string(REGEX REPLACE "${RPGSH_INSTALL_DIR_MACRO}[^\n]+" "${RPGSH_INSTALL_DIR_MACRO}\"${CMAKE_INSTALL_PREFIX}/bin/\"" TEXT "${TEXT}" )
file(GENERATE OUTPUT ${HEADERS_DIR}define.h CONTENT "${TEXT}")

#Library objects
foreach(lib ${libs})
	message("Adding ${lib} library")
	add_library(${lib} SHARED ${HEADERS_DIR}${lib}.h ${SRC_DIR}${lib}.cpp)
	target_include_directories(${lib} PRIVATE ${HEADERS_DIR} ${SRC_DIR})
	set_target_properties(${lib} PROPERTIES
				     BUILD_RPATH "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
				     INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endforeach()

#Binaries
message("\nAdding main binary")
add_executable(rpgsh ${SRC_DIR}main.cpp)
target_link_libraries(rpgsh PUBLIC ${libs})
set_target_properties(rpgsh PROPERTIES
			    BUILD_RPATH "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
			    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
list(APPEND prefixed_bins "rpgsh")
foreach(bin ${bins})
	message("Adding ${bin} binary")
	add_executable(${prefix}${bin} ${SRC_DIR}${bin}.cpp)
	target_link_libraries(${prefix}${bin} PUBLIC ${libs})
	set_target_properties(${prefix}${bin} PROPERTIES
					      BUILD_RPATH "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
					      INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
	list(APPEND prefixed_bins "${prefix}${bin}")
endforeach()

#Install
install(TARGETS ${prefixed_bins} ${libs}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib)

#Done!
message("\nDone!")
