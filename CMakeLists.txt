# Cmake params
cmake_minimum_required(VERSION 3.9)
# Export data for LSP
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# CPP params
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()
string(APPEND CMAKE_CXX_FLAGS " -Wall -Wextra -pedantic -Werror")
# string(APPEND CMAKE_CXX_FLAGS " -Rpass-missed=loop-vectorize")
# string(APPEND CMAKE_CXX_FLAGS " -Rpass-analysis=loop-vectorize")

# Set cmake standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

# Speed up
set(UNITY_BUILD true)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
	string(APPEND CMAKE_CXX_FLAGS_DEBUG " -fno-omit-frame-pointer -fsanitize=address")
	string(APPEND CMAKE_LINKER_FLAGS_DEBUG " -fno-omit-frame-pointer -fsanitize=address")
	add_link_options(-fsanitize=address)

	# string(APPEND CMAKE_CXX_FLAGS_DEBUG " -fsanitize=undefined")
	# string(APPEND CMAKE_LINKER_FLAGS_DEBUG " -fsanitize=undefined")
	# add_link_options(-fsanitize=undefined)

	# string(APPEND CMAKE_CXX_FLAGS_DEBUG " -ftime-trace")
endif()

# Define project
project(app CXX)
file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.cpp) 
include_directories(includes)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} algs) 


# Add libs
add_subdirectory(../../algs libs/algs)

# SFML
# find_package(SFML 2 REQUIRED network audio graphics window system)

find_package(SFML 2.5 COMPONENTS graphics REQUIRED)
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-audio)

# # Use threads
find_package(Threads)
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})

# # Use openmp if found
# option(USE_OPENMP "flag to use OpenMP for multithreading" ON)
# if(USE_OPENMP)
#     find_package(OpenMP 3.0 COMPONENTS CXX)
#     if (OpenMP_CXX_FOUND)
#         message(STATUS "Building with OpenMP Multithreading.")
#     else()
#         message(STATUS "No OpenMP found, no multithreading.")
#     endif()
# endif()

# Colored output
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
   add_compile_options (-fdiagnostics-color=always)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
   add_compile_options (-fcolor-diagnostics)
endif ()

###########################
#  compile_commands.json  #
###########################

add_custom_target(
    copy-compile-commands
    ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_CURRENT_LIST_DIR}
    )
