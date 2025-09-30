cmake_minimum_required(VERSION 3.16)

project(veyon-chat-plugin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Network)
find_package(PkgConfig REQUIRED)

# Find Veyon
pkg_check_modules(VEYON REQUIRED veyon-core)

# Include directories
include_directories(${VEYON_INCLUDE_DIRS})
include_directories(src)

# Set up Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Source files
set(SOURCES
    src/ChatFeaturePlugin.cpp
    src/ChatMasterWidget.cpp
    src/ChatClientWidget.cpp
    src/ChatMessage.cpp
    src/ChatSession.cpp
    src/ChatServiceClient.cpp
)

# Header files
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

# Link libraries
target_link_libraries(veyon-chat-plugin
    Qt5::Core
    Qt5::Widgets
    Qt5::Network
    ${VEYON_LIBRARIES}
)

# Set plugin properties
set_target_properties(veyon-chat-plugin PROPERTIES
    PREFIX ""
    SUFFIX ".so"
)

# Install the plugin
install(TARGETS veyon-chat-plugin
    DESTINATION ${VEYON_PLUGIN_DIR}
)
