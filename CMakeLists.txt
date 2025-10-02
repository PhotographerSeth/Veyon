cmake_minimum_required(VERSION 3.16)

project(veyon-chat-plugin VERSION 1.0.0 LANGUAGES CXX)

# C++ & Qt
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Network Multimedia)

# Path to the real Veyon 4.9.7 source tree (provided by CI or your local env)
set(VEYON_SOURCE_DIR "" CACHE PATH "Path to the Veyon sources (4.9.7)")
if(NOT VEYON_SOURCE_DIR)
  if(DEFINED ENV{VEYON_SOURCE_DIR})
    set(VEYON_SOURCE_DIR $ENV{VEYON_SOURCE_DIR})
  else()
    message(FATAL_ERROR "Set VEYON_SOURCE_DIR to the path of the Veyon sources (4.9.7).")
  endif()
endif()


# Enable Qt MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Set UI files directory
set(CMAKE_AUTOUIC_SEARCH_PATHS ui)

# Include project headers
set(PROJECT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Source files (ONLY the .cpp files that actually exist in your repository)
set(SOURCES
    src/ChatFeaturePlugin.cpp
    src/ChatMasterWidget.cpp
    src/ChatClientWidget.cpp
    src/ChatMessage.cpp
    src/ChatSession.cpp
    src/ChatServiceClient.cpp
    src/ChatSignalListener.cpp
    src/ChatRequestWorker.cpp
)

# Header files (for MOC)
set(HEADERS
    src/ChatFeaturePlugin.h
    src/ChatMasterWidget.h
    src/ChatClientWidget.h
    src/ChatMessage.h
    src/ChatSession.h
    src/ChatServiceClient.h
    src/ChatSignalListener.h
    src/ChatRequestWorker.h
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

# Use Veyon's real headers (interfaces live under src/ and src/plugins/)
target_include_directories(veyon-chat-plugin PRIVATE
    ${PROJECT_INCLUDE_DIR}
    ${VEYON_SOURCE_DIR}/src
    ${VEYON_SOURCE_DIR}/src/plugins
)

# Link Qt and user32 (for global hotkey)
target_link_libraries(veyon-chat-plugin PRIVATE
    Qt5::Core Qt5::Widgets Qt5::Network Qt5::Multimedia
    user32
)

# Set plugin properties
set_target_properties(veyon-chat-plugin PROPERTIES
    OUTPUT_NAME "veyon-chat-plugin"
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
