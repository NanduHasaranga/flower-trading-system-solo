cmake_minimum_required(VERSION 3.20)
project(FlowerTradingSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(exchange_lib STATIC
	src/Core/OrderValidator.cpp
	src/Engine/OrderBook.cpp
	src/IO/CsvReader.cpp
	src/IO/CsvWriter.cpp
	src/IO/OrderProcessor.cpp
	src/Utils/OrderIdGenerator.cpp
	src/Utils/TimeUtils.cpp
)
target_include_directories(exchange_lib PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(exchange_app src/main.cpp)
target_link_libraries(exchange_app PRIVATE exchange_lib)

target_compile_options(exchange_app
	PRIVATE
		$<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:GNU,Clang>>:-O3>
		$<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:MSVC>>:/O2>
)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

include(FetchContent)
FetchContent_Declare(
	googletest
	URL
	https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(unit_tests
	tests/unit/test_fixed_string.cpp
	tests/unit/test_order_validator.cpp
	tests/unit/test_order_book_side.cpp
	tests/unit/test_order_book.cpp
	tests/unit/test_order_processor.cpp
	tests/unit/test_exchange.cpp
)
target_link_libraries(unit_tests PRIVATE exchange_lib GTest::gtest_main)
target_include_directories(unit_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

include(GoogleTest)
gtest_discover_tests(unit_tests)
