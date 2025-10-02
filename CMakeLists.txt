cmake_minimum_required(VERSION 3.16)

project(veyon-chat-plugin VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt5 components (Core, Widgets, Network, Multimedia)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Network Multimedia)

# Enable Qt MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Set UI files directory
set(CMAKE_AUTOUIC_SEARCH_PATHS ui)

# Configure Veyon SDK paths
set(VEYON_SDK_ROOT "C:/src/veyon-5.1.0" CACHE PATH "Root directory of the installed Veyon SDK")
set(VEYON_SDK_INCLUDE_DIR "${VEYON_SDK_ROOT}/src" CACHE PATH "Path to the Veyon SDK core headers")
set(VEYON_SDK_PLUGIN_INCLUDE_DIR "${VEYON_SDK_ROOT}/src/plugins" CACHE PATH "Path to the Veyon SDK plugin headers")
set(VEYON_SDK_LIBRARY_DIR "${VEYON_SDK_ROOT}/build" CACHE PATH "Path containing the compiled Veyon SDK libraries")

if(NOT EXISTS "${VEYON_SDK_INCLUDE_DIR}")
    message(FATAL_ERROR "Veyon SDK include directory not found: ${VEYON_SDK_INCLUDE_DIR}. Please adjust VEYON_SDK_INCLUDE_DIR to point to the installed Veyon sources.")
endif()

if(NOT EXISTS "${VEYON_SDK_PLUGIN_INCLUDE_DIR}")
    message(FATAL_ERROR "Veyon SDK plugin include directory not found: ${VEYON_SDK_PLUGIN_INCLUDE_DIR}. Please adjust VEYON_SDK_PLUGIN_INCLUDE_DIR to point to the installed Veyon plugin interfaces.")
endif()

if(NOT EXISTS "${VEYON_SDK_LIBRARY_DIR}")
    message(FATAL_ERROR "Veyon SDK library directory not found: ${VEYON_SDK_LIBRARY_DIR}. Please adjust VEYON_SDK_LIBRARY_DIR to point to the directory containing the compiled Veyon libraries.")
endif()

# Source files (ONLY the .cpp files that actually exist in your repository)
set(SOURCES
    src/ChatFeaturePlugin.cpp
    src/ChatMasterWidget.cpp
    src/ChatClientWidget.cpp
    src/ChatMessage.cpp
    src/ChatSession.cpp
    src/ChatServiceClient.cpp
)

# Header files (for MOC)
set(HEADERS
    src/ChatFeaturePlugin.h
    src/ChatMasterWidget.h
    src/ChatClientWidget.h
    src/ChatMessage.h
    src/ChatSession.h
    src/ChatServiceClient.h
)

# UI files
set(UI_FILES
    ui/ChatMasterWidget.ui
    ui/ChatClientWidget.ui
)

# Resource files
set(RESOURCES
    resources/chat.qrc
)

# Create the plugin library
add_library(veyon-chat-plugin SHARED
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
    ${RESOURCES}
)

target_include_directories(veyon-chat-plugin
    PRIVATE
        src
        ${VEYON_SDK_INCLUDE_DIR}
        ${VEYON_SDK_PLUGIN_INCLUDE_DIR}
)

target_link_directories(veyon-chat-plugin
    PRIVATE
        ${VEYON_SDK_LIBRARY_DIR}
)

set(VEYON_SDK_LIBRARIES
    "VeyonCore;VeyonMaster;VeyonService;VeyonWorker;VeyonComputerControl"
    CACHE STRING "Semicolon-separated list of Veyon SDK libraries required by the plugin"
)

# Link Qt libraries
target_link_libraries(veyon-chat-plugin
    PRIVATE
        Qt5::Core
        Qt5::Widgets
        Qt5::Network
        Qt5::Multimedia
        ${VEYON_SDK_LIBRARIES}
)

# Set plugin properties
set_target_properties(veyon-chat-plugin PROPERTIES
    PREFIX ""
    SUFFIX ".dll"
)

# Windows-specific settings
if(WIN32)
    set_target_properties(veyon-chat-plugin PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Release"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Debug"
    )
endif()

# Print build information
message(STATUS "Building Veyon Chat Plugin v${PROJECT_VERSION}")
message(STATUS "Qt5 Core: ${Qt5Core_VERSION}")
message(STATUS "Qt5 Widgets: ${Qt5Widgets_VERSION}")
message(STATUS "Qt5 Network: ${Qt5Network_VERSION}")
message(STATUS "Qt5 Multimedia: ${Qt5Multimedia_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
