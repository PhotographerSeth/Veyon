cmake_minimum_required(VERSION 3.16)

project(veyon-chat-plugin VERSION 1.0.0 LANGUAGES CXX)

# Require C++17 (Qt 5.x is fine with this)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt 5.15.x to match Veyon 4.9.7
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Network Multimedia)

# ---- Veyon headers (from a local checkout of v4.9.7) ----
# Pass -DVEYON_SRC_DIR=... from the GitHub Actions configure step
set(VEYON_SRC_DIR "" CACHE PATH "Path to the Veyon 4.9.7 source tree")

if(NOT VEYON_SRC_DIR)
  message(FATAL_ERROR "Set -DVEYON_SRC_DIR to a Veyon v4.9.7 checkout (e.g. ${CMAKE_SOURCE_DIR}/veyon-src)")
endif()

# Find where the headers actually live in that tree
find_path(VEYON_FEATURE_DIR
  NAMES Feature.h
  HINTS ${VEYON_SRC_DIR}
  PATH_SUFFIXES src src/plugins src/core plugins core include include/veyon
  NO_DEFAULT_PATH
)
find_path(VEYON_PLUGINIFACE_DIR
  NAMES PluginInterface.h
  HINTS ${VEYON_SRC_DIR}
  PATH_SUFFIXES src src/plugins src/core plugins core include include/veyon
  NO_DEFAULT_PATH
)
find_path(VEYON_FEATUREPROV_DIR
  NAMES FeatureProviderInterface.h
  HINTS ${VEYON_SRC_DIR}
  PATH_SUFFIXES src src/plugins src/core plugins core include include/veyon
  NO_DEFAULT_PATH
)
find_path(VEYON_FEATUREMSG_DIR
  NAMES FeatureMessage.h
  HINTS ${VEYON_SRC_DIR}
  PATH_SUFFIXES src src/plugins src/core plugins core include include/veyon
  NO_DEFAULT_PATH
)
find_path(VEYON_SERVERIFACE_DIR
  NAMES VeyonServerInterface.h
  HINTS ${VEYON_SRC_DIR}
  PATH_SUFFIXES src src/master src/server master server include include/veyon
  NO_DEFAULT_PATH
)
find_path(VEYON_WORKERIFACE_DIR
  NAMES VeyonWorkerInterface.h
  HINTS ${VEYON_SRC_DIR}
  PATH_SUFFIXES src src/service src/worker service worker include include/veyon
  NO_DEFAULT_PATH
)
find_path(VEYON_COMPUTERCONTROL_DIR
  NAMES ComputerControlInterface.h
  HINTS ${VEYON_SRC_DIR}
  PATH_SUFFIXES src src/master src/server master server include include/veyon
  NO_DEFAULT_PATH
)

if(NOT VEYON_FEATURE_DIR)
  message(FATAL_ERROR "Feature.h not found under ${VEYON_SRC_DIR}")
endif()
if(NOT VEYON_PLUGINIFACE_DIR)
  message(FATAL_ERROR "PluginInterface.h not found under ${VEYON_SRC_DIR}")
endif()
if(NOT VEYON_FEATUREPROV_DIR)
  message(FATAL_ERROR "FeatureProviderInterface.h not found under ${VEYON_SRC_DIR}")
endif()
if(NOT VEYON_FEATUREMSG_DIR)
  message(FATAL_ERROR "FeatureMessage.h not found under ${VEYON_SRC_DIR}")
endif()
if(NOT VEYON_SERVERIFACE_DIR)
  message(FATAL_ERROR "VeyonServerInterface.h not found under ${VEYON_SRC_DIR}")
endif()
if(NOT VEYON_WORKERIFACE_DIR)
  message(FATAL_ERROR "VeyonWorkerInterface.h not found under ${VEYON_SRC_DIR}")
endif()
if(NOT VEYON_COMPUTERCONTROL_DIR)
  message(FATAL_ERROR "ComputerControlInterface.h not found under ${VEYON_SRC_DIR}")
endif()

message(STATUS "Using Veyon headers:")
message(STATUS "  Feature.h:                 ${VEYON_FEATURE_DIR}")
message(STATUS "  PluginInterface.h:         ${VEYON_PLUGINIFACE_DIR}")
message(STATUS "  FeatureProviderInterface.h:${VEYON_FEATUREPROV_DIR}")
message(STATUS "  FeatureMessage.h:          ${VEYON_FEATUREMSG_DIR}")
message(STATUS "  VeyonServerInterface.h:    ${VEYON_SERVERIFACE_DIR}")
message(STATUS "  VeyonWorkerInterface.h:    ${VEYON_WORKERIFACE_DIR}")
message(STATUS "  ComputerControlInterface.h:${VEYON_COMPUTERCONTROL_DIR}")

# Add those include folders so #include \"Feature.h\" etc. resolve
# (Keep your existing include dirs for your own sources/resources)
include_directories(
  ${VEYON_FEATURE_DIR}
  ${VEYON_PLUGINIFACE_DIR}
  ${VEYON_FEATUREPROV_DIR}
  ${VEYON_FEATUREMSG_DIR}
  ${VEYON_SERVERIFACE_DIR}
  ${VEYON_WORKERIFACE_DIR}
  ${VEYON_COMPUTERCONTROL_DIR}
)


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

# Use project headers
target_include_directories(veyon-chat-plugin PRIVATE
    ${PROJECT_INCLUDE_DIR}
)

# Link Qt and user32 (for global hotkey)
target_link_libraries(veyon-chat-plugin PRIVATE
    Qt5::Core
    Qt5::Widgets
    Qt5::Network
    Qt5::Multimedia
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
# Report Qt component versions for diagnostics
foreach(_comp IN ITEMS Core Widgets Network Multimedia)
  string(CONCAT _qt_version_var "Qt5" "${_comp}" "_VERSION")
  if(DEFINED ${_qt_version_var})
    message(STATUS "Qt ${_comp}: ${${_qt_version_var}}")
  endif()
endforeach()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
