get_filename_component(FLOWER_TRADING_REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

set(FLOWER_TRADING_QT_MIN_VERSION 6.5)
set(FLOWER_TRADING_INCLUDE_DIR "${FLOWER_TRADING_REPO_ROOT}/include")
set(FLOWER_TRADING_GUI_INCLUDE_DIR "${FLOWER_TRADING_REPO_ROOT}/traderApp")
set(FLOWER_TRADING_CLI_MAIN "${FLOWER_TRADING_REPO_ROOT}/src/main.cpp")

set(FLOWER_TRADING_CORE_SOURCES
    "${FLOWER_TRADING_REPO_ROOT}/src/Core/OrderValidator.cpp"
    "${FLOWER_TRADING_REPO_ROOT}/src/Engine/OrderBook.cpp"
    "${FLOWER_TRADING_REPO_ROOT}/src/IO/CsvReader.cpp"
    "${FLOWER_TRADING_REPO_ROOT}/src/IO/CsvWriter.cpp"
    "${FLOWER_TRADING_REPO_ROOT}/src/IO/OrderProcessor.cpp"
    "${FLOWER_TRADING_REPO_ROOT}/src/Utils/OrderIdGenerator.cpp"
    "${FLOWER_TRADING_REPO_ROOT}/src/Utils/TimeUtils.cpp"
)

set(FLOWER_TRADING_GUI_SOURCES
    "${FLOWER_TRADING_REPO_ROOT}/traderApp/main.cpp"
    "${FLOWER_TRADING_REPO_ROOT}/traderApp/mainwindow.cpp"
    "${FLOWER_TRADING_REPO_ROOT}/traderApp/mainwindow.h"
    "${FLOWER_TRADING_REPO_ROOT}/traderApp/mainwindow.ui"
)

function(flower_trading_deploy_qt_runtime target_name)
    if(NOT WIN32)
        return()
    endif()

    find_program(FLOWER_TRADING_WINDEPLOYQT_EXECUTABLE
        NAMES windeployqt windeployqt6
        HINTS "${QT6_INSTALL_PREFIX}/bin"
        REQUIRED
    )

    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND "${FLOWER_TRADING_WINDEPLOYQT_EXECUTABLE}"
            --compiler-runtime
            --dir "$<TARGET_FILE_DIR:${target_name}>"
            "$<TARGET_FILE:${target_name}>"
        COMMENT "Deploying Qt runtime dependencies for ${target_name}"
        VERBATIM
    )
endfunction()