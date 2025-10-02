cmake_minimum_required(VERSION 3.16)

project(veyon-chat-plugin VERSION 1.0.0 LANGUAGES CXX)

# C++ & Qt
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt 5/6 neutral finder
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets Network Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Network Multimedia)

# Allow Actions (or local builds) to point at a checked-out Veyon tree
set(VEYON_SRC_DIR "" CACHE PATH "Path to the Veyon sources")
if(NOT DEFINED VEYON_SRC_DIR AND DEFINED ENV{VEYON_SRC_DIR})
  set(VEYON_SRC_DIR "$ENV{VEYON_SRC_DIR}")
endif()


# Enable Qt MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Set UI files directory
set(CMAKE_AUTOUIC_SEARCH_PATHS ui)

# Include project headers
set(PROJECT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Collect plausible include roots for both old/new layouts
set(_veyon_inc_candidates
  "${VEYON_SRC_DIR}/plugins"              "${VEYON_SRC_DIR}/src/plugins"
  "${VEYON_SRC_DIR}/core"                 "${VEYON_SRC_DIR}/src/core"
  "${VEYON_SRC_DIR}/master"               "${VEYON_SRC_DIR}/src/master"
  "${VEYON_SRC_DIR}/common"               "${VEYON_SRC_DIR}/src/common"
  "${VEYON_SRC_DIR}/include"              "${VEYON_SRC_DIR}/src/include"
)
foreach(_d IN LISTS _veyon_inc_candidates)
  if(EXISTS "${_d}")
    list(APPEND VEYON_INCLUDE_DIRS "${_d}")
  endif()
endforeach()
message(STATUS "Using Veyon includes: ${VEYON_INCLUDE_DIRS}")

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
    ${VEYON_INCLUDE_DIRS}
)

# Link Qt and user32 (for global hotkey)
target_link_libraries(veyon-chat-plugin PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Multimedia
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
set(_qt_components Core Widgets Network Multimedia)
foreach(_comp IN LISTS _qt_components)
  string(CONCAT _qt_version_var "Qt" "${QT_VERSION_MAJOR}" "${_comp}" "_VERSION")
  message(STATUS "Qt ${_comp}: ${${_qt_version_var}}")
endforeach()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
