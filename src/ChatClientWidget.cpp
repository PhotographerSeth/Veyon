#include "ChatClientWidget.h"

#include <QApplication>
#include <QAction>
#include <QDateTime>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QString>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QSettings>
#include <QShortcut>
#include <QSystemTrayIcon>
#include <QTextCursor>
#include <QTextEdit>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>

namespace {
constexpr auto ORGANIZATION_NAME = "Veyon";
constexpr auto APPLICATION_NAME = "ChatClient";
constexpr auto SETTINGS_GEOMETRY = "geometry";
constexpr auto SETTINGS_SOUND = "soundEnabled";
constexpr auto CLIENT_ID = "client";
constexpr auto MASTER_ID = "master";
}

ChatClientWidget::ChatClientWidget(QWidget* parent) :
    QWidget(parent),
    m_chatDisplay(nullptr),
    m_messageInput(nullptr),
    m_sendButton(nullptr),
    m_statusLabel(nullptr),
    m_trayIcon(nullptr),
    m_f10Shortcut(nullptr),
    m_sendShortcut(nullptr),
    m_typingTimer(new QTimer(this)),
    m_notificationSound(new QSoundEffect(this)),
    m_soundEnabled(true),
    m_clientId(QString::fromUtf8(CLIENT_ID)),
    m_unreadCount(0)
{
    setObjectName(QStringLiteral("ChatClientWidget"));
    setupUI();
    setupTrayIcon();
    setupShortcuts();
    loadSettings();

    m_typingTimer->setInterval(2000);
    m_typingTimer->setSingleShot(true);
    connect(m_typingTimer, &QTimer::timeout, this, &ChatClientWidget::onTypingTimer);

    connect(m_sendButton, &QPushButton::clicked, this, &ChatClientWidget::onSendButtonClicked);
    connect(m_messageInput, &QLineEdit::textChanged, this, &ChatClientWidget::onMessageInputChanged);

    if (m_trayIcon) {
        connect(m_trayIcon, &QSystemTrayIcon::activated,
                this, &ChatClientWidget::onTrayIconActivated);
    }

    connect(m_notificationSound, &QSoundEffect::statusChanged, this, [this]() {
        const auto status = m_notificationSound->status();

        if (status == QSoundEffect::Ready || status == QSoundEffect::Null) {
            playNotificationSound();
        }
    });

    setWindowTitle(tr("Veyon Chat"));
    resize(500, 400);
}

ChatClientWidget::~ChatClientWidget()
{
    saveSettings();
}

void ChatClientWidget::receiveMessage(const ChatMessage& message)
{
    addMessageToDisplay(message);
    ++m_unreadCount;
    updateWindowTitle();

    if (m_trayIcon && !isActiveWindow()) {
        m_trayIcon->showMessage(tr("New message"),
                                tr("Message from %1").arg(message.senderId()),
                                QIcon(QStringLiteral(":/chat/icons/chat.png")));
    }

    playNotificationSound();
}

void ChatClientWidget::clearChat()
{
    m_chatDisplay->clear();
    m_unreadCount = 0;
    updateWindowTitle();
}

void ChatClientWidget::setMasterName(const QString& name)
{
    m_masterName = name;
    updateWindowTitle();
}

void ChatClientWidget::setClientId(const QString& clientId)
{
    m_clientId = clientId;
}

void ChatClientWidget::closeEvent(QCloseEvent* event)
{
    saveSettings();
    QWidget::closeEvent(event);
}

void ChatClientWidget::changeEvent(QEvent* event)
{
    QWidget::changeEvent(event);
    if (event->type() == QEvent::LanguageChange) {
        updateWindowTitle();
    }
}

void ChatClientWidget::onSendButtonClicked()
{
    const QString content = m_messageInput->text().trimmed();
    if (content.isEmpty()) {
        return;
    }

    ChatMessage message(m_clientId, MASTER_ID, content, ChatMessage::Priority::Normal);
    addMessageToDisplay(message);
    emit sendMessage(message);

    m_messageInput->clear();
    m_typingTimer->stop();
}

void ChatClientWidget::onMessageInputChanged()
{
    const bool hasText = !m_messageInput->text().trimmed().isEmpty();
    m_sendButton->setEnabled(hasText);

    if (hasText) {
        emit statusChanged(ChatSession::ClientStatus::Typing);
        m_typingTimer->start();
    }
}

void ChatClientWidget::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
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

void ChatClientWidget::onTypingTimer()
{
    emit statusChanged(ChatSession::ClientStatus::Online);
}

void ChatClientWidget::playNotificationSound()
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

void ChatClientWidget::setupUI()
{
    auto* layout = new QVBoxLayout(this);

    m_statusLabel = new QLabel(this);
    layout->addWidget(m_statusLabel);

    m_chatDisplay = new QTextEdit(this);
    m_chatDisplay->setReadOnly(true);
    layout->addWidget(m_chatDisplay, 1);

    auto* inputLayout = new QHBoxLayout();
    m_messageInput = new QLineEdit(this);
    m_messageInput->setPlaceholderText(tr("Type a message"));
    inputLayout->addWidget(m_messageInput, 1);

    m_sendButton = new QPushButton(QIcon(QStringLiteral(":/chat/icons/send.png")), tr("Send"), this);
    m_sendButton->setEnabled(false);
    inputLayout->addWidget(m_sendButton);

    layout->addLayout(inputLayout);

    updateWindowTitle();
}

void ChatClientWidget::setupTrayIcon()
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
    connect(hideAction, &QAction::triggered, this, &ChatClientWidget::hide);
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

    m_trayIcon->setContextMenu(menu);
    m_trayIcon->show();
}

void ChatClientWidget::setupShortcuts()
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
    connect(m_sendShortcut, &QShortcut::activated, this, &ChatClientWidget::onSendButtonClicked);
}

void ChatClientWidget::loadSettings()
{
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
    if (settings.contains(SETTINGS_GEOMETRY)) {
        restoreGeometry(settings.value(SETTINGS_GEOMETRY).toByteArray());
    }
    m_soundEnabled = settings.value(SETTINGS_SOUND, true).toBool();
}

void ChatClientWidget::saveSettings()
{
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
    settings.setValue(SETTINGS_GEOMETRY, saveGeometry());
    settings.setValue(SETTINGS_SOUND, m_soundEnabled);
}

void ChatClientWidget::addMessageToDisplay(const ChatMessage& message)
{
    const QString text = formatMessage(message);
    m_chatDisplay->append(text);
    scrollToBottom();
}

void ChatClientWidget::scrollToBottom()
{
    QTextCursor cursor = m_chatDisplay->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_chatDisplay->setTextCursor(cursor);
}

void ChatClientWidget::updateWindowTitle()
{
    QString title = tr("Chat with %1").arg(m_masterName.isEmpty() ? tr("Master") : m_masterName);
    if (m_unreadCount > 0) {
        title.append(tr(" - %1 new"));
        title = title.arg(m_unreadCount);
    }
    setWindowTitle(title);
    m_statusLabel->setText(title);
}

QString ChatClientWidget::formatMessage(const ChatMessage& message) const
{
    const QString time = message.formattedTimestamp();
    const QString sender = message.senderId() == MASTER_ID ? tr("Master") : tr("You");
    return tr("[%1] %2: %3").arg(time, sender, message.content());
}
