cmake_minimum_required(VERSION 3.20)
project(FlowerTradingSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(exchange_app
    src/main.cpp
    src/Core/OrderValidator.cpp
    src/Engine/OrderBook.cpp
    src/IO/CsvReader.cpp
    src/IO/CsvWriter.cpp
    src/IO/OrderProcessor.cpp
    src/Utils/OrderIdGenerator.cpp
    src/Utils/TimeUtils.cpp
)

target_include_directories(exchange_app PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Explicit Release optimization flags per compiler
target_compile_options(exchange_app PRIVATE
    $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:GNU,Clang>>:-O3>
    $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:MSVC>>:/O2>
)

# Release build uses -O3 by default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
