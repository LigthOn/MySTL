cmake_minimum_required(VERSION 2.8)

project(MySTL)

# version
set(MySTL_VERSION_MAJOR 1)
set(MySTL_VERSION_MINOR 0)
set(MySTL_VERSION_PATCH 0)
set(MySTL_VERSION "${MySTL_VERSION_MAJOR}.${MySTL_VERSION_MINOR}.${MySTL_VERSION_PATCH}")
message(STATUS "The version of this project is: ${MyTinySTL_VERSION}")

set(CMAKE_BUILD_TYPE debug)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Wextra -Wno-sign-compare -Wno-unused-but-set-variable -Wno-array-bounds")
	# set(EXTRA_CXX_FLAGS -Weffc++ -Wswitch-default -Wfloat-equal -Wconversion -Wsign-conversion)
	if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "5.0.0")
		message(FATAL_ERROR "required GCC 5.0 or later")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	endif()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Wextra -Wno-sign-compare")
	# set(EXTRA_CXX_FLAGS -Weffc++ -Wswitch-default -Wfloat-equal -Wconversion -Wimplicit-fallthrough)
	if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "3.5.0")
		message(FATAL_ERROR "required Clang 3.5 or later")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	endif()
endif()

message(STATUS "The cmake_cxx_flags is: ${CMAKE_CXX_FLAGS}")

# add_subdirectory(${PROJECT_SOURCE_DIR}/Test)
