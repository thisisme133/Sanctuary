cmake_minimum_required(VERSION 3.28)
project(Sanctuary VERSION 1.0.0 LANGUAGES CXX)

# C++23 Standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags for C++23 and performance
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall -Wextra -Wpedantic
        -march=native
        -fno-rtti  # RTTI not needed, improves performance
    )
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O3 -flto)
        add_link_options(-flto)
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(
        /W4 /permissive-
        /utf-8  # Unicode support for fmt
        /GR-  # Disable RTTI
    )
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(/O2 /GL /Gw)
        add_link_options(/LTCG)
    endif()
endif()

# Find required packages
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

# Optional packages - gracefully degrade if not found
find_package(nlohmann_json QUIET)
find_package(SQLite3 QUIET)
find_package(Boost QUIET COMPONENTS system thread)
find_package(spdlog QUIET)
find_package(cxxopts QUIET)
find_package(fmt QUIET)

# Create interface targets for missing dependencies
if(NOT nlohmann_json_FOUND)
    add_library(nlohmann_json INTERFACE)
    target_compile_definitions(nlohmann_json INTERFACE NO_JSON_LIB)
endif()

if(NOT SQLite3_FOUND)
    add_library(SQLite3 INTERFACE)
    target_compile_definitions(SQLite3 INTERFACE NO_SQLITE_LIB)
endif()

if(NOT fmt_FOUND)
    add_library(fmt INTERFACE)
    target_compile_definitions(fmt INTERFACE NO_FMT_LIB)
endif()

if(NOT spdlog_FOUND)
    add_library(spdlog INTERFACE)
    target_compile_definitions(spdlog INTERFACE NO_SPDLOG_LIB)
endif()

if(NOT cxxopts_FOUND)
    add_library(cxxopts INTERFACE)
    target_compile_definitions(cxxopts INTERFACE NO_CXXOPTS_LIB)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/cpp/include)

# Add subdirectories for each module (in dependency order!)
# Core first (no dependencies)
if(EXISTS ${CMAKE_SOURCE_DIR}/cpp/Sanctuary.Core/CMakeLists.txt)
    add_subdirectory(cpp/Sanctuary.Core)
endif()

# Entities (depends on Core)
if(EXISTS ${CMAKE_SOURCE_DIR}/cpp/Sanctuary.Database.Entities/CMakeLists.txt)
    add_subdirectory(cpp/Sanctuary.Database.Entities)
endif()

# Database (depends on Core + Entities)
if(EXISTS ${CMAKE_SOURCE_DIR}/cpp/Sanctuary.Database/CMakeLists.txt AND SQLite3_FOUND)
    add_subdirectory(cpp/Sanctuary.Database)
endif()

# Packet structures (depend on Core)
if(EXISTS ${CMAKE_SOURCE_DIR}/cpp/Sanctuary.Packet.Common/CMakeLists.txt)
    add_subdirectory(cpp/Sanctuary.Packet.Common)
endif()
if(EXISTS ${CMAKE_SOURCE_DIR}/cpp/Sanctuary.Packet/CMakeLists.txt)
    add_subdirectory(cpp/Sanctuary.Packet)
endif()

# UDP Library (depends on Core)
if(EXISTS ${CMAKE_SOURCE_DIR}/cpp/Sanctuary.UdpLibrary/CMakeLists.txt)
    add_subdirectory(cpp/Sanctuary.UdpLibrary)
endif()

# Game logic (depends on Core)
if(EXISTS ${CMAKE_SOURCE_DIR}/cpp/Sanctuary.Game/CMakeLists.txt)
    add_subdirectory(cpp/Sanctuary.Game)
endif()

# Servers (depend on Core + UDP + Packet)
if(EXISTS ${CMAKE_SOURCE_DIR}/cpp/Sanctuary.Gateway/CMakeLists.txt)
    add_subdirectory(cpp/Sanctuary.Gateway)
endif()
if(EXISTS ${CMAKE_SOURCE_DIR}/cpp/Sanctuary.Login/CMakeLists.txt)
    add_subdirectory(cpp/Sanctuary.Login)
endif()

# WebAPI (depends on Core + Database)
if(EXISTS ${CMAKE_SOURCE_DIR}/cpp/Sanctuary.WebAPI/CMakeLists.txt AND Boost_FOUND)
    add_subdirectory(cpp/Sanctuary.WebAPI)
endif()

# Enable testing
if(EXISTS ${CMAKE_SOURCE_DIR}/cpp/Sanctuary.UdpLibrary.Tests/CMakeLists.txt)
    enable_testing()
    add_subdirectory(cpp/Sanctuary.UdpLibrary.Tests)
endif()
