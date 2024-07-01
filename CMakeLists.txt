cmake_minimum_required(VERSION 3.21)

option(GENERATE_UNION_API_LIB "This option generates Union API static library target. By default this option is set to ON" ON)
option(GENERATE_UNION_API_DLL "This option generates Union API dynamic library target. By default this option is set to OFF" OFF)
option(GENERATE_UNION_API_EXE "This option generates Union API executable target. By default this option is set to OFF" OFF)

project(UnionAPI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SRC
	"union-api/*.h"
	"union-api/*.cpp"
)

## Union API static lib
if (GENERATE_UNION_API_LIB)
	add_library(union_api_lib STATIC ${SRC})

	target_sources(union_api_lib PRIVATE ${SRC})

	target_compile_definitions(union_api_lib
		PUBLIC
			_UNION_API_LIB
		PRIVATE
			_UNION_API_BUILD
			_CRT_STDIO_ARBITRARY_WIDE_SPECIFIERS
	)

	target_include_directories(union_api_lib PUBLIC "union-api/")
endif()

# Union API dynamic lib
if (GENERATE_UNION_API_DLL)
	add_library(union_api_dll SHARED)

	target_sources(union_api_dll PRIVATE ${SRC})

	target_compile_definitions(union_api_dll
		PUBLIC
			_UNION_API_DLL
		PRIVATE
			_UNION_API_BUILD
	)

	target_include_directories(union_api_dll PUBLIC "union-api/")
endif()

## Union API executable
if (GENERATE_UNION_API_EXE)
	add_executable(union_api_exe)

	target_sources(union_api_exe PRIVATE ${SRC})

	target_compile_definitions(union_api_exe
		PUBLIC
			_UNION_API_EXE
		PRIVATE
			_UNION_API_BUILD
	)

	target_include_directories(union_api_exe PUBLIC "union-api/")
endif()

# dependencies
add_subdirectory(dependencies)