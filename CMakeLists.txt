cmake_minimum_required(VERSION 3.16)

project(MiraApp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.2 REQUIRED COMPONENTS Core Gui Quick Network Charts Widgets)

# --- HEADER (INCLUDE) YOLLARI ---
include_directories(
    src/core
    src/network
    src/config
)

# --- KAYNAK DOSYALAR ---
set(PROJECT_SOURCES
    src/main.cpp

    # Core
    src/core/MiraController.h
    src/core/MiraController.cpp

    # Network
    src/network/TelemetryClient.h
    src/network/TelemetryClient.cpp

    # Config
    src/config/ConfigManager.h
    src/config/ConfigManager.cpp
)

qt_add_executable(MiraApp
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    ui/main.qml
)


target_link_libraries(MiraApp PRIVATE
    Qt6::Core Qt6::Gui Qt6::Quick Qt6::Network Qt6::Charts Qt6::Widgets
)

# QML Modülü
qt_add_qml_module(MiraApp
    URI "Mira"
    VERSION 1.0
    QML_FILES ui/main.qml
    SOURCES src/core/MiraController.h src/core/MiraController.cpp
)

# Config dosyasını kopyala
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/mira_config.ini DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MiraApp)
endif()
