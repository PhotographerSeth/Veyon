#include "ChatMasterWidget.h"

#include <QApplication>
#include <QAbstractItemView>
#include <QAction>
#include <QtCore/qobjectdefs.h>
#include <QComboBox>
#include <QDateTime>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QShortcut>
#include <QSplitter>
#include <QSystemTrayIcon>
#include <QTextCursor>
#include <QTextEdit>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>

namespace {
constexpr auto ORGANIZATION_NAME = "Veyon";
constexpr auto APPLICATION_NAME = "ChatMaster";
constexpr auto SETTINGS_GEOMETRY = "geometry";
constexpr auto SETTINGS_SOUND = "soundEnabled";
constexpr auto MASTER_ID = "master";

ChatMessage::Priority priorityFromIndex(int index)
{
    switch (index) {
    case 1: return ChatMessage::Priority::Urgent;
    case 2: return ChatMessage::Priority::Announcement;
    default: return ChatMessage::Priority::Normal;
    }
}

} // namespace

ChatMasterWidget::ChatMasterWidget(QWidget* parent) :
    QWidget(parent),
    m_splitter(nullptr),
    m_clientList(nullptr),
    m_chatDisplay(nullptr),
    m_messageInput(nullptr),
    m_sendButton(nullptr),
    m_clearButton(nullptr),
    m_globalButton(nullptr),
    m_priorityCombo(nullptr),
    m_quickReplies(nullptr),
    m_statusLabel(nullptr),
    m_trayIcon(nullptr),
    m_f10Shortcut(nullptr),
    m_sendShortcut(nullptr),
    m_typingTimer(new QTimer(this)),
    m_notificationSound(new QSoundEffect(this)),
    m_soundEnabled(true)
{
    setObjectName(QStringLiteral("ChatMasterWidget"));
    setupUI();
    setupTrayIcon();
    setupShortcuts();
    setupQuickReplies();
    loadSettings();

    m_typingTimer->setInterval(2000);
    m_typingTimer->setSingleShot(true);
    connect(m_typingTimer, &QTimer::timeout, this, &ChatMasterWidget::onTypingTimer);

    connect(m_sendButton, &QPushButton::clicked, this, &ChatMasterWidget::onSendButtonClicked);
    connect(m_clearButton, &QPushButton::clicked, this, &ChatMasterWidget::onClearChatClicked);
    connect(m_globalButton, &QPushButton::clicked, this, &ChatMasterWidget::onGlobalBroadcastClicked);

    connect(m_messageInput, &QLineEdit::textChanged, this, &ChatMasterWidget::onMessageInputChanged);

    connect(m_clientList, &QListWidget::currentRowChanged, this, [this](int) {
        onClientSelectionChanged();
    });

    if (m_trayIcon) {
        connect(m_trayIcon, &QSystemTrayIcon::activated,
                this, &ChatMasterWidget::onTrayIconActivated);
    }

    connect(m_notificationSound, &QSoundEffect::statusChanged, this, [this]() {
        const auto status = m_notificationSound->status();

        if (status == QSoundEffect::Ready || status == QSoundEffect::Null) {
            playNotificationSound();
        }
    });

    setWindowTitle(tr("Veyon Chat - Master"));
    resize(900, 600);
}

ChatMasterWidget::~ChatMasterWidget()
{
    saveSettings();
}

void ChatMasterWidget::addClient(const QString& clientId, const QString& clientName)
{
    if (!m_sessions.contains(clientId)) {
        m_sessions.insert(clientId, ChatSession(clientId));
    }

    ChatSession& session = m_sessions[clientId];
    session.setClientName(clientName);
    updateClientList();
}

void ChatMasterWidget::removeClient(const QString& clientId)
{
    m_sessions.remove(clientId);

    if (m_currentClientId == clientId) {
        m_currentClientId.clear();
        m_chatDisplay->clear();
    }

    updateClientList();
}

void ChatMasterWidget::updateClientStatus(const QString& clientId, ChatSession::ClientStatus status)
{
    if (!m_sessions.contains(clientId)) {
        m_sessions.insert(clientId, ChatSession(clientId));
    }

    m_sessions[clientId].setStatus(status);
    updateClientList();
}

void ChatMasterWidget::focusClient(const QString& clientId)
{
    if (clientId.isEmpty()) {
        return;
    }

    if (!m_sessions.contains(clientId)) {
        ChatSession session(clientId);
        session.setClientName(clientId);
        m_sessions.insert(clientId, session);
    }

    m_currentClientId = clientId;
    updateClientList();

    for (int i = 0; i < m_clientList->count(); ++i) {
        if (QListWidgetItem* item = m_clientList->item(i)) {
            if (item->data(Qt::UserRole).toString().compare(clientId, Qt::CaseInsensitive) == 0) {
                m_clientList->setCurrentItem(item);
                break;
            }
        }
    }

    updateChatDisplay();
}

void ChatMasterWidget::receiveMessage(const ChatMessage& message)
{
    const QString clientId = message.senderId();
    if (!m_sessions.contains(clientId)) {
        m_sessions.insert(clientId, ChatSession(clientId));
    }

    ChatSession& session = m_sessions[clientId];
    session.addMessage(message);

    if (m_currentClientId.isEmpty()) {
        m_currentClientId = clientId;
    }

    if (m_currentClientId == clientId) {
        addMessageToDisplay(message);
        session.markAllAsRead();
    }

    if (m_trayIcon && !isActiveWindow()) {
        m_trayIcon->showMessage(tr("New message"),
                                tr("Message from %1").arg(session.clientName()),
                                QIcon(QStringLiteral(":/chat/icons/chat.png")));
    }

    playNotificationSound();
    updateClientList();
}

void ChatMasterWidget::updateMessageStatus(const QString& messageId, ChatMessage::Status status)
{
    for (auto it = m_sessions.begin(); it != m_sessions.end(); ++it) {
        auto history = it->history();
        bool updated = false;
        for (auto& message : history) {
            if (message.messageId() == messageId) {
                message.setStatus(status);
                updated = true;
            }
        }
        if (updated) {
            it->clearHistory();
            for (const auto& msg : history) {
                it->addMessage(msg);
            }
        }
    }

    if (!m_currentClientId.isEmpty()) {
        updateChatDisplay();
    }
}

void ChatMasterWidget::setMasterName(const QString& name)
{
    m_masterName = name;
    setWindowTitle(tr("Veyon Chat - %1").arg(m_masterName));
}

void ChatMasterWidget::closeEvent(QCloseEvent* event)
{
    saveSettings();
    QWidget::closeEvent(event);
}

void ChatMasterWidget::changeEvent(QEvent* event)
{
    QWidget::changeEvent(event);
    if (event->type() == QEvent::LanguageChange) {
        setWindowTitle(tr("Veyon Chat - Master"));
    }
}

void ChatMasterWidget::onClientSelectionChanged()
{
    const QString newClientId = getSelectedClientId();
    if (newClientId == m_currentClientId) {
        return;
    }

    m_currentClientId = newClientId;

    if (ChatSession* session = getCurrentSession()) {
        session->markAllAsRead();
    }

    updateChatDisplay();
    updateClientList();
}

void ChatMasterWidget::onSendButtonClicked()
{
    const QString clientId = getSelectedClientId();
    if (clientId.isEmpty()) {
        QMessageBox::information(this, tr("Select client"), tr("Please select a client before sending a message."));
        return;
    }

    const QString content = m_messageInput->text().trimmed();
    if (content.isEmpty()) {
        return;
    }

    ChatMessage message(MASTER_ID, clientId, content, priorityFromIndex(m_priorityCombo->currentIndex()));
    addMessageToDisplay(message);

    if (!m_sessions.contains(clientId)) {
        m_sessions.insert(clientId, ChatSession(clientId));
    }

    m_sessions[clientId].addMessage(message);
    m_sessions[clientId].markAllAsRead();

    emit sendMessage(message);

    m_messageInput->clear();
    m_typingTimer->stop();
    updateClientList();
}

void ChatMasterWidget::onClearChatClicked()
{
    const QString clientId = getSelectedClientId();
    if (clientId.isEmpty()) {
        return;
    }

    if (auto* session = getCurrentSession()) {
        session->clearHistory();
    }

    m_chatDisplay->clear();
    emit clearClientChat(clientId);
    updateClientList();
}

void ChatMasterWidget::onGlobalBroadcastClicked()
{
    const QString content = m_messageInput->text().trimmed();
    if (content.isEmpty()) {
        return;
    }

    const auto priority = priorityFromIndex(m_priorityCombo->currentIndex());
    emit sendGlobalMessage(content, priority);

    ChatMessage broadcast(MASTER_ID, QStringLiteral("*"), content, priority);
    addMessageToDisplay(broadcast);

    for (auto it = m_sessions.begin(); it != m_sessions.end(); ++it) {
        it->addMessage(broadcast);
    }

    m_messageInput->clear();
    updateClientList();
}

void ChatMasterWidget::onMessageInputChanged()
{
    const bool hasText = !m_messageInput->text().trimmed().isEmpty();
    m_sendButton->setEnabled(hasText);
    m_globalButton->setEnabled(hasText);

    if (hasText) {
        if (auto* session = getCurrentSession()) {
            session->setStatus(ChatSession::ClientStatus::Typing);
            updateClientList();
        }
        m_typingTimer->start();
    }
}

void ChatMasterWidget::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
        if (isHidden()) {
            showNormal();
            activateWindow();
        } else {
            hide();
        }
    }
}

void ChatMasterWidget::onTypingTimer()
{
    if (auto* session = getCurrentSession()) {
        session->setStatus(ChatSession::ClientStatus::Online);
        updateClientList();
    }
}

void ChatMasterWidget::playNotificationSound()
{
    if (!m_soundEnabled) {
        return;
    }

    if (m_notificationSound->status() == QSoundEffect::Ready) {
        m_notificationSound->play();
    } else if (m_notificationSound->status() == QSoundEffect::Null) {
        m_notificationSound->setSource(QUrl(QStringLiteral("qrc:/chat/sounds/notification.wav")));
        m_notificationSound->setVolume(0.6);
        m_notificationSound->play();
    }
}

void ChatMasterWidget::setupUI()
{
    auto* mainLayout = new QVBoxLayout(this);

    m_statusLabel = new QLabel(tr("No client selected"), this);
    mainLayout->addWidget(m_statusLabel);

    m_splitter = new QSplitter(Qt::Horizontal, this);
    mainLayout->addWidget(m_splitter, 1);

    auto* leftWidget = new QWidget(this);
    auto* leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    m_clientList = new QListWidget(leftWidget);
    m_clientList->setSelectionMode(QAbstractItemView::SingleSelection);
    leftLayout->addWidget(m_clientList);

    auto* rightWidget = new QWidget(this);
    auto* rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setContentsMargins(0, 0, 0, 0);

    m_chatDisplay = new QTextEdit(rightWidget);
    m_chatDisplay->setReadOnly(true);
    rightLayout->addWidget(m_chatDisplay, 1);

    auto* quickLayout = new QHBoxLayout();
    m_quickReplies = new QComboBox(rightWidget);
    m_quickReplies->setToolTip(tr("Quick replies"));
    quickLayout->addWidget(m_quickReplies);

    m_priorityCombo = new QComboBox(rightWidget);
    m_priorityCombo->addItem(tr("Normal"));
    m_priorityCombo->addItem(tr("Urgent"));
    m_priorityCombo->addItem(tr("Announcement"));
    quickLayout->addWidget(m_priorityCombo);
    rightLayout->addLayout(quickLayout);

    auto* inputLayout = new QHBoxLayout();
    m_messageInput = new QLineEdit(rightWidget);
    m_messageInput->setPlaceholderText(tr("Type a message"));
    inputLayout->addWidget(m_messageInput, 1);

    m_sendButton = new QPushButton(QIcon(QStringLiteral(":/chat/icons/send.png")), tr("Send"), rightWidget);
    m_sendButton->setEnabled(false);
    inputLayout->addWidget(m_sendButton);

    m_clearButton = new QPushButton(QIcon(QStringLiteral(":/chat/icons/clear.png")), tr("Clear"), rightWidget);
    inputLayout->addWidget(m_clearButton);

    m_globalButton = new QPushButton(QIcon(QStringLiteral(":/chat/icons/broadcast.png")), tr("Broadcast"), rightWidget);
    m_globalButton->setEnabled(false);
    inputLayout->addWidget(m_globalButton);

    rightLayout->addLayout(inputLayout);

    m_splitter->addWidget(leftWidget);
    m_splitter->addWidget(rightWidget);
    m_splitter->setStretchFactor(0, 1);
    m_splitter->setStretchFactor(1, 2);

    connect(m_quickReplies, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        if (index <= 0) {
            return;
        }
        const QString text = m_quickReplies->currentText();
        m_messageInput->setText(text);
        m_messageInput->setFocus();
    });
}

void ChatMasterWidget::setupTrayIcon()
{
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        return;
    }

    m_trayIcon = new QSystemTrayIcon(QIcon(QStringLiteral(":/chat/icons/chat.png")), this);

    auto* menu = new QMenu(this);
    QAction* showAction = menu->addAction(tr("Show"));
    QAction* hideAction = menu->addAction(tr("Hide"));
    menu->addSeparator();
    QAction* quitAction = menu->addAction(tr("Quit"));

    connect(showAction, &QAction::triggered, this, [this]() {
        showNormal();
        activateWindow();
    });
    connect(hideAction, &QAction::triggered, this, &ChatMasterWidget::hide);
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

    m_trayIcon->setContextMenu(menu);
    m_trayIcon->show();
}

void ChatMasterWidget::setupShortcuts()
{
    m_f10Shortcut = new QShortcut(QKeySequence(Qt::Key_F10), this);
    m_f10Shortcut->setContext(Qt::ApplicationShortcut);
    connect(m_f10Shortcut, &QShortcut::activated, this, [this]() {
        if (isHidden()) {
            showNormal();
            activateWindow();
        } else {
            hide();
        }
    });

    m_sendShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Return), this);
    connect(m_sendShortcut, &QShortcut::activated, this, &ChatMasterWidget::onSendButtonClicked);
}

void ChatMasterWidget::setupQuickReplies()
{
    m_quickReplies->clear();
    m_quickReplies->addItem(tr("Quick replies"));
    m_quickReplies->addItem(tr("Please pay attention to the main screen."));
    m_quickReplies->addItem(tr("The lesson is about to start."));
    m_quickReplies->addItem(tr("Do you need any help?"));
}

void ChatMasterWidget::loadSettings()
{
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
    if (settings.contains(SETTINGS_GEOMETRY)) {
        restoreGeometry(settings.value(SETTINGS_GEOMETRY).toByteArray());
    }
    m_soundEnabled = settings.value(SETTINGS_SOUND, true).toBool();
}

void ChatMasterWidget::saveSettings()
{
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
    settings.setValue(SETTINGS_GEOMETRY, saveGeometry());
    settings.setValue(SETTINGS_SOUND, m_soundEnabled);
}

void ChatMasterWidget::updateClientList()
{
    const QString previousSelection = getSelectedClientId();

    m_clientList->clear();
    for (auto it = m_sessions.constBegin(); it != m_sessions.constEnd(); ++it) {
        const ChatSession& session = it.value();
        QString label = session.clientName();
        if (session.hasUnreadMessages()) {
            label += tr(" (%1 new)").arg(session.unreadCount());
        }
        QListWidgetItem* item = new QListWidgetItem(label, m_clientList);
        item->setData(Qt::UserRole, it.key());
        item->setToolTip(session.statusString());
        if (it.key() == m_currentClientId) {
            item->setSelected(true);
        }
    }

    if (!previousSelection.isEmpty()) {
        for (int i = 0; i < m_clientList->count(); ++i) {
            QListWidgetItem* item = m_clientList->item(i);
            if (item->data(Qt::UserRole).toString() == previousSelection) {
                m_clientList->setCurrentItem(item);
                break;
            }
        }
    }

    if (m_currentClientId.isEmpty() && m_clientList->count() > 0) {
        m_clientList->setCurrentRow(0);
    }

    if (auto* session = getCurrentSession()) {
        m_statusLabel->setText(tr("Selected: %1 (%2)")
                                   .arg(session->clientName(), session->statusString()));
    } else {
        m_statusLabel->setText(tr("No client selected"));
    }
}

void ChatMasterWidget::updateChatDisplay()
{
    m_chatDisplay->clear();
    if (auto* session = getCurrentSession()) {
        const auto history = session->history();
        for (const ChatMessage& message : history) {
            addMessageToDisplay(message);
        }
    }
}

void ChatMasterWidget::addMessageToDisplay(const ChatMessage& message)
{
    const QString text = formatMessage(message);
    m_chatDisplay->append(text);
    scrollToBottom();
}

void ChatMasterWidget::scrollToBottom()
{
    QTextCursor cursor = m_chatDisplay->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_chatDisplay->setTextCursor(cursor);
}

QString ChatMasterWidget::formatMessage(const ChatMessage& message) const
{
    const QString time = message.formattedTimestamp();
    const QString sender = message.senderId() == MASTER_ID ? tr("Master") : message.senderId();
    const QString priority = message.priorityString();
    return tr("[%1] %2 (%3): %4").arg(time, sender, priority, message.content());
}

QString ChatMasterWidget::getSelectedClientId() const
{
    if (QListWidgetItem* item = m_clientList->currentItem()) {
        return item->data(Qt::UserRole).toString();
    }
    return m_currentClientId;
}

ChatSession* ChatMasterWidget::getCurrentSession()
{
    if (m_currentClientId.isEmpty()) {
        return nullptr;
    }

    auto it = m_sessions.find(m_currentClientId);
    if (it == m_sessions.end()) {
        return nullptr;
    }

    return &it.value();
}
