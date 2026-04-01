cmake_minimum_required(VERSION 3.20)
project(TraderApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/FlowerTradingSources.cmake)

find_package(Qt6 ${FLOWER_TRADING_QT_MIN_VERSION} QUIET COMPONENTS Core Widgets)
if(NOT Qt6_FOUND)
    message(FATAL_ERROR
        "TraderApp requires Qt 6.5+ with the Core and Widgets modules.\n"
        "Set CMAKE_PREFIX_PATH or Qt6_DIR to the Qt installation prefix that contains Qt6Config.cmake.\n"
        "Example: -DCMAKE_PREFIX_PATH=C:/Qt/6.5.3/mingw_64"
    )
endif()

function(flower_trading_enable_release_optimizations target_name)
    target_compile_options(${target_name} PRIVATE
        $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:GNU,Clang>>:-O3>
        $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:MSVC>>:/O2>
    )
endfunction()

add_library(flower_trading_core STATIC ${FLOWER_TRADING_CORE_SOURCES})
target_include_directories(flower_trading_core PUBLIC ${FLOWER_TRADING_INCLUDE_DIR})
flower_trading_enable_release_optimizations(flower_trading_core)

qt_standard_project_setup()

qt_add_executable(TraderApp
    WIN32 MACOSX_BUNDLE
    ${FLOWER_TRADING_GUI_SOURCES}
)

target_include_directories(TraderApp
    PRIVATE
        ${FLOWER_TRADING_GUI_INCLUDE_DIR}
)

target_link_libraries(TraderApp
    PRIVATE
        flower_trading_core
        Qt::Core
        Qt::Widgets
)
flower_trading_enable_release_optimizations(TraderApp)
flower_trading_deploy_qt_runtime(TraderApp)

include(GNUInstallDirs)

install(TARGETS TraderApp
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET TraderApp
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})