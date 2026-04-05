cmake_minimum_required(VERSION 3.16)

project(moe_control VERSION 0.1 LANGUAGES CXX)

if(ANDROID)
    if(DEFINED CMAKE_ANDROID_NDK_VERSION AND CMAKE_ANDROID_NDK_VERSION VERSION_LESS 26.1)
        message(FATAL_ERROR "Android NDK ${CMAKE_ANDROID_NDK_VERSION} is too old. Please use NDK 26.1+ (recommended: 26.1.10909125 or newer).")
    endif()

    # 防止 Qt Creator/缓存导致“编译器是一个 NDK，toolchain file 却是另一个 NDK”的混用。
    # 这种情况下会触发 libc++ 头文件链错误（<cstdint>/<cmath> 等报错）。
    if(DEFINED CMAKE_TOOLCHAIN_FILE AND DEFINED ANDROID_NDK)
        file(TO_CMAKE_PATH "${CMAKE_TOOLCHAIN_FILE}" _toolchain_file_norm)
        file(TO_CMAKE_PATH "${ANDROID_NDK}" _android_ndk_norm)
        string(FIND "${_toolchain_file_norm}" "${_android_ndk_norm}" _ndk_match_pos)
        if(_ndk_match_pos EQUAL -1)
            message(FATAL_ERROR
                "Android NDK mismatch detected:\n"
                "  ANDROID_NDK       = ${ANDROID_NDK}\n"
                "  CMAKE_TOOLCHAIN_FILE = ${CMAKE_TOOLCHAIN_FILE}\n"
                "Please make sure both point to the same NDK (26.1+), then delete build dir and reconfigure.")
        endif()
    endif()

    set(CMAKE_ANDROID_STL_TYPE c++_shared)

    # AndroidX (core 1.13+) requires compileSdk >= 34.
    set(QT_ANDROID_MIN_SDK_VERSION 28)
    set(QT_ANDROID_TARGET_SDK_VERSION 35)
    set(QT_ANDROID_COMPILE_SDK_VERSION 35)

    if(DEFINED ANDROID_PLATFORM)
        string(REGEX REPLACE "^android-" "" _android_api "${ANDROID_PLATFORM}")
        if(_android_api VERSION_LESS 34)
            message(FATAL_ERROR
                "ANDROID_PLATFORM is ${ANDROID_PLATFORM}. This project requires compileSdk >= 34 (recommended: android-35). "
                "Please change the Android platform in Qt Creator build step from android-33 to android-35, then reconfigure.")
        endif()
    endif()
endif()

string(TIMESTAMP BUILD_TIME "%Y.%m.%d-%H%M" UTC)
find_package(Git QUIET)
if(Git_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
endif()
if(NOT GIT_HASH)
    set(GIT_HASH "nogit")
endif()
set(APP_BUILD_INFO "${PROJECT_VERSION}-${BUILD_TIME}-${GIT_HASH}")

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 REQUIRED COMPONENTS Quick Network LinguistTools)

include(FetchContent)

# Android平台需要特殊配置spdlog
if(ANDROID)
    set(SPDLOG_FMT_EXTERNAL OFF CACHE BOOL "" FORCE)
    set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "" FORCE)
endif()

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.12.0
)
FetchContent_MakeAvailable(spdlog)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appmoe_control
    main.cpp
    tcp_client.h
    tcp_client.cpp
    system_data_model.h
    system_data_model.cpp
    logger.h
    logger.cpp
    settings_manager.h
    settings_manager.cpp
)

target_compile_definitions(appmoe_control PRIVATE
    APP_BUILD_INFO="${APP_BUILD_INFO}"
)

qt_add_qml_module(appmoe_control
    URI moe_control
    VERSION 1.0
    QML_FILES
        Main.qml
        SettingsDialog.qml
    SOURCES
        tcp_client.h tcp_client.cpp
        system_data_model.h system_data_model.cpp
        logger.h logger.cpp
        settings_manager.h settings_manager.cpp
)

set(TS_FILES
    translations/moe_control_zh_CN.ts
)

qt_add_translations(appmoe_control
    TS_FILES ${TS_FILES}
    RESOURCE_PREFIX "/i18n"
    SOURCES
        main.cpp
        Main.qml
        SettingsDialog.qml
        tcp_client.cpp tcp_client.h
        system_data_model.cpp system_data_model.h
        settings_manager.cpp settings_manager.h
        logger.cpp logger.h
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appmoe_control PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appmoe_control
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Android specific settings
if(ANDROID)
    set_target_properties(appmoe_control PROPERTIES
        QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
        QT_ANDROID_MIN_SDK_VERSION ${QT_ANDROID_MIN_SDK_VERSION}
        QT_ANDROID_TARGET_SDK_VERSION ${QT_ANDROID_TARGET_SDK_VERSION}
        QT_ANDROID_COMPILE_SDK_VERSION ${QT_ANDROID_COMPILE_SDK_VERSION}
    )
    
    target_compile_options(appmoe_control PRIVATE
        -fexceptions
        -frtti
    )
    
    target_compile_definitions(appmoe_control PRIVATE
        SPDLOG_HEADER_ONLY
        SPDLOG_FMT_EXTERNAL_HO
    )
endif()

target_link_libraries(appmoe_control
    PRIVATE Qt6::Quick Qt6::Network
)

if(ANDROID)
    target_include_directories(appmoe_control PRIVATE ${spdlog_SOURCE_DIR}/include)
else()
    target_link_libraries(appmoe_control PRIVATE spdlog::spdlog)
endif()

include(GNUInstallDirs)
install(TARGETS appmoe_control
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
