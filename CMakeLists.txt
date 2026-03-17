cmake_minimum_required(VERSION 3.16)

project(moe_control VERSION 0.1 LANGUAGES CXX)

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

qt_add_qml_module(appmoe_control
    URI moe_control
    VERSION 1.0
    QML_FILES
        main.qml
        settings_dialog.qml
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
