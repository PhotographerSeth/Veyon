cmake_minimum_required(VERSION 3.16)

project(veyon-chat-plugin VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find Qt5 components (Core, Widgets, Network, Multimedia)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Network Multimedia)

# Enable Qt MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Set UI files directory
set(CMAKE_AUTOUIC_SEARCH_PATHS ui)

# Create mock Veyon headers directory
set(MOCK_HEADERS_DIR "${CMAKE_CURRENT_BINARY_DIR}/mock-headers")
file(MAKE_DIRECTORY "${MOCK_HEADERS_DIR}")

# Create Feature.h mock header (missing from previous version)
file(WRITE "${MOCK_HEADERS_DIR}/Feature.h"
"#pragma once
#include <QString>
#include <QKeySequence>

class Feature {
public:
    typedef QString Uid;
    enum Flag { 
        None = 0, Mode = 1, Action = 2, Session = 4, Meta = 8, 
        Option = 16, Checked = 32, Master = 256, Service = 512, 
        Worker = 1024, Builtin = 4096 
    };
    
    Feature() = default;
    Feature(Uid uid, int flags, QString name, QString displayName, 
            QString displayNameActive, QString description, 
            QString iconUrl, QKeySequence shortcut)
        : m_uid(uid), m_flags(flags), m_name(name), m_displayName(displayName) {}
    
    Uid uid() const { return m_uid; }
    int flags() const { return m_flags; }
    QString name() const { return m_name; }
    QString displayName() const { return m_displayName; }
    
private:
    Uid m_uid;
    int m_flags = 0;
    QString m_name;
    QString m_displayName;
};

typedef QList<Feature> FeatureList;
")

# Create comprehensive mock Veyon headers with proper Qt interface declarations
file(WRITE "${MOCK_HEADERS_DIR}/FeatureProviderInterface.h"
"#pragma once
#include <QObject>
#include <QVersionNumber>
#include <QVariantMap>
#include <QList>
#include <QString>
#include <QKeySequence>
#include \"Feature.h\"
#include \"FeatureMessage.h\"
#include \"VeyonServerInterface.h\"
#include \"VeyonWorkerInterface.h\"
#include \"ComputerControlInterface.h\"

class FeatureProviderInterface {
public:
    enum Operation { Start, Stop };
    virtual ~FeatureProviderInterface() = default;
    virtual const FeatureList& featureList() const = 0;
    virtual bool controlFeature(Feature::Uid, Operation, const QVariantMap&, const ComputerControlInterfaceList&) = 0;
    virtual bool handleFeatureMessage(VeyonServerInterface&, const MessageContext&, const FeatureMessage&) = 0;
    virtual bool handleFeatureMessage(VeyonWorkerInterface&, const FeatureMessage&) = 0;
};

// Qt interface declaration for MOC
Q_DECLARE_INTERFACE(FeatureProviderInterface, \"org.veyon.FeatureProviderInterface\")
")

file(WRITE "${MOCK_HEADERS_DIR}/PluginInterface.h"
"#pragma once
#include <QObject>
#include <QVersionNumber>
#include <QString>

class Plugin {
public:
    typedef QString Uid;
};

class PluginInterface {
public:
    virtual ~PluginInterface() = default;
    virtual Plugin::Uid uid() const = 0;
    virtual QVersionNumber version() const = 0;
    virtual QString name() const = 0;
    virtual QString description() const = 0;
    virtual QString vendor() const = 0;
    virtual QString copyright() const = 0;
    virtual QString shortName() const = 0;
    virtual void upgrade(const QVersionNumber&) = 0;
};

// Qt interface declaration for MOC
Q_DECLARE_INTERFACE(PluginInterface, \"org.veyon.PluginInterface\")
")

# Include mock headers
include_directories("${MOCK_HEADERS_DIR}")
include_directories(src)

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

# Link Qt libraries
target_link_libraries(veyon-chat-plugin
    Qt5::Core
    Qt5::Widgets
    Qt5::Network
    Qt5::Multimedia
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
