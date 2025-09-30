cmake_minimum_required(VERSION 3.16)

project(veyon-chat-plugin VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt5 components (Core, Widgets, Network only - no multimedia needed)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Network)

# Enable Qt MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Set UI files directory
set(CMAKE_AUTOUIC_SEARCH_PATHS ui)

# Create mock Veyon headers directory
set(MOCK_HEADERS_DIR "${CMAKE_CURRENT_BINARY_DIR}/mock-headers")
file(MAKE_DIRECTORY "${MOCK_HEADERS_DIR}")

# Create comprehensive mock Veyon headers
file(WRITE "${MOCK_HEADERS_DIR}/FeatureProviderInterface.h"
"#pragma once
#include <QObject>
#include <QVersionNumber>
#include <QVariantMap>
#include <QList>
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
    
    Feature(Uid uid, int flags, QString name, QString displayName, 
            QString displayNameActive, QString description, 
            QString iconUrl, QKeySequence shortcut)
        : m_uid(uid), m_flags(flags), m_name(name) {}
    
    Uid uid() const { return m_uid; }
    
private:
    Uid m_uid;
    int m_flags;
    QString m_name;
};

typedef QList<Feature> FeatureList;

class Computer {
public:
    QString hostAddress() const { return QString(); }
};

class ComputerControlInterface {
public:
    Computer computer() const { return Computer(); }
    void sendFeatureMessage(const class FeatureMessage&, bool) {}
};
typedef QList<ComputerControlInterface*> ComputerControlInterfaceList;

class MessageContext {};
class VeyonServerInterface {};
class VeyonWorkerInterface {};

class FeatureMessage {
public:
    FeatureMessage(const QString& uid, int command) {}
    void addArgument(const QString& key, const QVariant& value) {}
    QVariant argument(const QString& key) const { return QVariant(); }
    QString featureUid() const { return QString(); }
    int command() const { return 0; }
};

class FeatureProviderInterface {
public:
    enum Operation { Start, Stop };
    virtual ~FeatureProviderInterface() = default;
    virtual const FeatureList& featureList() const = 0;
    virtual bool controlFeature(Feature::Uid, Operation, const QVariantMap&, const ComputerControlInterfaceList&) = 0;
    virtual bool handleFeatureMessage(VeyonServerInterface&, const MessageContext&, const FeatureMessage&) = 0;
    virtual bool handleFeatureMessage(VeyonWorkerInterface&, const FeatureMessage&) = 0;
};
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
")

# Include mock headers
include_directories("${MOCK_HEADERS_DIR}")
include_directories(src)

# Source files
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

# Link Qt libraries (Core, Widgets, Network only)
target_link_libraries(veyon-chat-plugin
    Qt5::Core
    Qt5::Widgets
    Qt5::Network
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
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")