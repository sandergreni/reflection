cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

project(reflection
	DESCRIPTION "Library that provide reflection using boost::pfr"
        LANGUAGES CXX)

add_library(reflection reflection.cpp)
add_library(json json.cpp)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

find_package(Boost 1.87.0 REQUIRED)
target_include_directories(reflection PRIVATE ${Boost_INCLUDE_DIRS})

include(rapidjson.cmake)
target_include_directories(json PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(json PRIVATE ${RAPIDJSON_INCLUDE_DIR})

option(BUILD_TESTS "Build unit tests" OFF)
if (BUILD_TESTS)
	include(FetchContent)
	FetchContent_Declare(
		GoogleTest
		GIT_REPOSITORY https://github.com/google/googletest.git
		GIT_TAG v1.16.0
	)
	# For Windows: Prevent overriding the parent project's compiler/linker settings
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(googletest)
	
	enable_testing()
	message(STATUS "Building tests...")
	add_executable(tests testing_reflection.cpp)
	target_include_directories(tests PRIVATE ${Boost_INCLUDE_DIRS})
	target_include_directories(tests PRIVATE ${RAPIDJSON_INCLUDE_DIR})
	target_link_libraries(tests GTest::gtest)
	target_link_libraries(tests json)
	include(GoogleTest)	
	
	gtest_discover_tests(tests)
endif()
