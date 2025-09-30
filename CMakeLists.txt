cmake_minimum_required(VERSION 3.16)

project(veyon-chat-plugin VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt5 components (Core, Widgets, Network only)
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

# Create comprehensive mock Veyon headers with proper Qt interface declarations
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

# Create the missing widget implementation files during build
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/ChatMasterWidget.cpp"
"#include \"ChatMasterWidget.h\"
#include \"ChatMessage.h\"
#include \"ChatSession.h\"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QSplitter>
#include <QComboBox>
#include <QDateTime>

ChatMasterWidget::ChatMasterWidget(QWidget* parent) : QWidget(parent) {
    setupUi();
}

void ChatMasterWidget::setupUi() {
    setWindowTitle(\"Veyon Chat - Master\");
    resize(800, 600);
    
    auto* layout = new QVBoxLayout(this);
    
    // Top toolbar
    auto* toolbarLayout = new QHBoxLayout();
    m_priorityCombo = new QComboBox();
    m_priorityCombo->addItems({\"Normal\", \"Urgent\", \"Announcement\"});
    toolbarLayout->addWidget(new QLabel(\"Priority:\"));
    toolbarLayout->addWidget(m_priorityCombo);
    
    m_clearButton = new QPushButton(\"Clear Chat\");
    m_broadcastButton = new QPushButton(\"Broadcast to All\");
    toolbarLayout->addWidget(m_clearButton);
    toolbarLayout->addWidget(m_broadcastButton);
    toolbarLayout->addStretch();
    
    layout->addLayout(toolbarLayout);
    
    // Main content
    auto* splitter = new QSplitter(Qt::Horizontal, this);
    
    // Client list
    auto* clientWidget = new QWidget();
    auto* clientLayout = new QVBoxLayout(clientWidget);
    clientLayout->addWidget(new QLabel(\"Connected Clients:\"));
    m_clientList = new QListWidget();
    clientLayout->addWidget(m_clientList);
    
    // Chat area
    auto* chatWidget = new QWidget();
    auto* chatLayout = new QVBoxLayout(chatWidget);
    chatLayout->addWidget(new QLabel(\"Chat Messages:\"));
    m_chatDisplay = new QTextEdit();
    m_chatDisplay->setReadOnly(true);
    chatLayout->addWidget(m_chatDisplay);
    
    auto* inputLayout = new QHBoxLayout();
    m_messageInput = new QLineEdit();
    m_messageInput->setPlaceholderText(\"Type your message here...\");
    m_sendButton = new QPushButton(\"Send\");
    inputLayout->addWidget(m_messageInput);
    inputLayout->addWidget(m_sendButton);
    chatLayout->addLayout(inputLayout);
    
    splitter->addWidget(clientWidget);
    splitter->addWidget(chatWidget);
    splitter->setSizes({200, 600});
    
    layout->addWidget(splitter);
    
    // Connect signals
    connect(m_sendButton, &QPushButton::clicked, this, &ChatMasterWidget::onSendClicked);
    connect(m_messageInput, &QLineEdit::returnPressed, this, &ChatMasterWidget::onSendClicked);
    connect(m_broadcastButton, &QPushButton::clicked, this, &ChatMasterWidget::onBroadcastClicked);
    connect(m_clearButton, &QPushButton::clicked, this, &ChatMasterWidget::onClearClicked);
}

void ChatMasterWidget::onSendClicked() {
    QString text = m_messageInput->text().trimmed();
    if (!text.isEmpty()) {
        auto priority = static_cast<ChatMessage::Priority>(m_priorityCombo->currentIndex());
        ChatMessage msg(\"master\", \"selected\", text, priority);
        emit sendMessage(msg);
        m_messageInput->clear();
        
        // Add to display
        QString priorityText = m_priorityCombo->currentText();
        m_chatDisplay->append(QString(\"[%1] [%2] Master: %3\")
            .arg(QDateTime::currentDateTime().toString(\"hh:mm:ss\"))
            .arg(priorityText)
            .arg(text));
    }
}

void ChatMasterWidget::onBroadcastClicked() {
    QString text = m_messageInput->text().trimmed();
    if (!text.isEmpty()) {
        auto priority = static_cast<ChatMessage::Priority>(m_priorityCombo->currentIndex());
        emit sendGlobalMessage(text, priority);
        m_messageInput->clear();
        
        // Add to display
        m_chatDisplay->append(QString(\"[%1] [BROADCAST] Master: %2\")
            .arg(QDateTime::currentDateTime().toString(\"hh:mm:ss\"))
            .arg(text));
    }
}

void ChatMasterWidget::onClearClicked() {
    m_chatDisplay->clear();
    emit clearClientChat(\"all\");
}

void ChatMasterWidget::receiveMessage(const ChatMessage& message) {
    QString priorityText;
    switch(message.priority()) {
        case ChatMessage::Urgent: priorityText = \"URGENT\"; break;
        case ChatMessage::Announcement: priorityText = \"ANNOUNCEMENT\"; break;
        default: priorityText = \"Normal\"; break;
    }
    
    m_chatDisplay->append(QString(\"[%1] [%2] %3: %4\")
        .arg(message.timestamp().toString(\"hh:mm:ss\"))
        .arg(priorityText)
        .arg(message.senderId())
        .arg(message.content()));
}

void ChatMasterWidget::updateClientStatus(const QString& clientId, ChatSession::ClientStatus status) {
    // Update client list with status indicators
    for (int i = 0; i < m_clientList->count(); ++i) {
        auto* item = m_clientList->item(i);
        if (item->text().contains(clientId)) {
            QString statusText;
            switch(status) {
                case ChatSession::Online: statusText = \"🟢\"; break;
                case ChatSession::Away: statusText = \"🟡\"; break;
                case ChatSession::Typing: statusText = \"✏️\"; break;
                default: statusText = \"⚫\"; break;
            }
            item->setText(QString(\"%1 %2\").arg(statusText, clientId));
            break;
        }
    }
}
")

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/ChatClientWidget.cpp"
"#include \"ChatClientWidget.h\"
#include \"ChatMessage.h\"
#include \"ChatSession.h\"
#include <QVBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QDateTime>

ChatClientWidget::ChatClientWidget(QWidget* parent) : QWidget(parent) {
    setupUi();
}

void ChatClientWidget::setupUi() {
    setWindowTitle(\"Veyon Chat - Student\");
    resize(400, 300);
    
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(\"Chat with Teacher:\"));
    
    m_chatDisplay = new QTextEdit();
    m_chatDisplay->setReadOnly(true);
    layout->addWidget(m_chatDisplay);
    
    auto* inputLayout = new QHBoxLayout();
    m_messageInput = new QLineEdit();
    m_messageInput->setPlaceholderText(\"Type your message to the teacher...\");
    m_sendButton = new QPushButton(\"Send Message\");
    inputLayout->addWidget(m_messageInput);
    inputLayout->addWidget(m_sendButton);
    layout->addLayout(inputLayout);
    
    connect(m_sendButton, &QPushButton::clicked, this, &ChatClientWidget::onSendClicked);
    connect(m_messageInput, &QLineEdit::returnPressed, this, &ChatClientWidget::onSendClicked);
}

void ChatClientWidget::onSendClicked() {
    QString text = m_messageInput->text().trimmed();
    if (!text.isEmpty()) {
        ChatMessage msg(\"student\", \"master\", text, ChatMessage::Normal);
        emit sendMessage(msg);
        m_messageInput->clear();
        
        // Add to display
        m_chatDisplay->append(QString(\"[%1] You: %2\")
            .arg(QDateTime::currentDateTime().toString(\"hh:mm:ss\"))
            .arg(text));
        
        emit statusChanged(ChatSession::Online);
    }
}

void ChatClientWidget::receiveMessage(const ChatMessage& message) {
    QString priorityPrefix;
    switch(message.priority()) {
        case ChatMessage::Urgent: priorityPrefix = \"[URGENT] \"; break;
        case ChatMessage::Announcement: priorityPrefix = \"[ANNOUNCEMENT] \"; break;
        default: priorityPrefix = \"\"; break;
    }
    
    m_chatDisplay->append(QString(\"[%1] %2Teacher: %3\")
        .arg(message.timestamp().toString(\"hh:mm:ss\"))
        .arg(priorityPrefix)
        .arg(message.content()));
}

void ChatClientWidget::clearChat() {
    m_chatDisplay->clear();
}
")

# Add the generated widget implementations to sources
list(APPEND SOURCES 
    "${CMAKE_CURRENT_BINARY_DIR}/ChatMasterWidget.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/ChatClientWidget.cpp"
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