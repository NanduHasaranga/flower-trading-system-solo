cmake_minimum_required(VERSION 3.20)
project(FlowerTradingSystem LANGUAGES CXX)

option(BUILD_GUI "Build the Qt GUI application when Qt6 is available" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/FlowerTradingSources.cmake)

function(flower_trading_enable_release_optimizations target_name)
    target_compile_options(${target_name} PRIVATE
        $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:GNU,Clang>>:-O3>
        $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:MSVC>>:/O2>
    )
endfunction()

add_library(flower_trading_core STATIC ${FLOWER_TRADING_CORE_SOURCES})
target_include_directories(flower_trading_core PUBLIC ${FLOWER_TRADING_INCLUDE_DIR})
flower_trading_enable_release_optimizations(flower_trading_core)

add_executable(exchange_app ${FLOWER_TRADING_CLI_MAIN})
target_link_libraries(exchange_app PRIVATE flower_trading_core)
flower_trading_enable_release_optimizations(exchange_app)

include(GNUInstallDirs)
install(TARGETS exchange_app
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(BUILD_GUI)
    find_package(Qt6 ${FLOWER_TRADING_QT_MIN_VERSION} QUIET COMPONENTS Core Widgets)
    if(NOT Qt6_FOUND)
        message(FATAL_ERROR
            "BUILD_GUI=ON requires Qt 6.5+ with the Core and Widgets modules.\n"
            "Set CMAKE_PREFIX_PATH or Qt6_DIR to the Qt installation prefix that contains Qt6Config.cmake.\n"
            "Example: -DCMAKE_PREFIX_PATH=C:/Qt/6.5.3/mingw_64"
        )
    endif()

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
endif()
