/*
 * ChatMasterWidget.cpp - implementation of ChatMasterWidget class
 *
 * Copyright (c) 2025 Manus AI <manus@example.com>
 *
 * This file is part of Veyon Chat Plugin - https://github.com/veyon/veyon-chat-plugin
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#include "ChatMasterWidget.h"

#include "ChatMessage.h"
#include "ChatSession.h"
#include "ui_ChatMasterWidget.h"

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QComboBox>
#include <QDateTime>
#include <QEvent>
#include <QIcon>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QPushButton>
#include <QScrollBar>
#include <QSettings>
#include <QShortcut>
#include <QSoundEffect>
#include <QSplitter>
#include <QSystemTrayIcon>
#include <QTextEdit>
#include <QTextOption>
#include <QTimer>
#include <QUrl>

namespace
{
QString statusToDisplayString(ChatSession::ClientStatus status)
{
    switch (status) {
    case ChatSession::ClientStatus::Online:
        return QObject::tr("Online");
    case ChatSession::ClientStatus::Away:
        return QObject::tr("Away");
    case ChatSession::ClientStatus::Typing:
        return QObject::tr("Typing…");
    }

    return QObject::tr("Online");
}

QString statusIconPath(ChatSession::ClientStatus status)
{
    switch (status) {
    case ChatSession::ClientStatus::Online:
        return QStringLiteral(":/chat/icons/online.png");
    case ChatSession::ClientStatus::Away:
        return QStringLiteral(":/chat/icons/away.png");
    case ChatSession::ClientStatus::Typing:
        return QStringLiteral(":/chat/icons/typing.png");
    }

    return QStringLiteral(":/chat/icons/online.png");
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
    setupUI();
    setupTrayIcon();
    setupShortcuts();
    setupQuickReplies();
    loadSettings();

    connect(m_typingTimer, &QTimer::timeout, this, &ChatMasterWidget::onTypingTimer);
    m_typingTimer->setSingleShot(true);
    m_typingTimer->setInterval(2000);

    updateClientList();
    updateChatDisplay();
}

ChatMasterWidget::~ChatMasterWidget()
{
    saveSettings();
}

void ChatMasterWidget::addClient(const QString& clientId, const QString& clientName)
{
    if (clientId.isEmpty()) {
        return;
    }

    auto it = m_sessions.find(clientId);
    if (it == m_sessions.end()) {
        ChatSession session(clientId);
        session.setClientName(clientName.isEmpty() ? clientId : clientName);
        m_sessions.insert(clientId, session);
    } else {
        it.value().setClientName(clientName.isEmpty() ? clientId : clientName);
        it.value().setStatus(ChatSession::ClientStatus::Online);
    }

    updateClientList();
}

void ChatMasterWidget::removeClient(const QString& clientId)
{
    if (clientId.isEmpty()) {
        return;
    }

    m_sessions.remove(clientId);

    if (m_currentClientId == clientId) {
        m_currentClientId.clear();
        updateChatDisplay();
    }

    updateClientList();
}

void ChatMasterWidget::updateClientStatus(const QString& clientId, ChatSession::ClientStatus status)
{
    if (clientId.isEmpty()) {
        return;
    }

    auto it = m_sessions.find(clientId);
    if (it == m_sessions.end()) {
        ChatSession session(clientId);
        session.setStatus(status);
        m_sessions.insert(clientId, session);
    } else {
        it.value().setStatus(status);
    }

    if (clientId == m_currentClientId && m_statusLabel) {
        const auto& session = m_sessions.value(clientId);
        m_statusLabel->setText(tr("%1 (%2)")
                                   .arg(session.clientName(), statusToDisplayString(session.status())));
    }

    updateClientList();
}

void ChatMasterWidget::receiveMessage(const ChatMessage& message)
{
    const QString clientId = message.senderId();
    if (clientId.isEmpty()) {
        return;
    }

    auto it = m_sessions.find(clientId);
    if (it == m_sessions.end()) {
        ChatSession session(clientId);
        session.addMessage(message);
        m_sessions.insert(clientId, session);
    } else {
        it.value().addMessage(message);
    }

    if (clientId == m_currentClientId) {
        addMessageToDisplay(message);
        if (auto* session = getCurrentSession()) {
            session->markAllAsRead();
        }
    } else {
        playNotificationSound();
        if (m_trayIcon && m_trayIcon->isVisible()) {
            m_trayIcon->showMessage(tr("Message from %1").arg(m_sessions.value(clientId).clientName()),
                                    message.content(), QSystemTrayIcon::Information, 3000);
        }
    }

    updateClientList();
}

void ChatMasterWidget::updateMessageStatus(const QString& messageId, ChatMessage::Status status)
{
    if (messageId.isEmpty()) {
        return;
    }

    bool updated = false;
    for (auto it = m_sessions.begin(); it != m_sessions.end(); ++it) {
        if (it.value().updateMessageStatus(messageId, status)) {
            updated = true;
            if (it.key() == m_currentClientId) {
                updateChatDisplay();
            }
            break;
        }
    }

    if (updated) {
        updateClientList();
    }
}

void ChatMasterWidget::setMasterName(const QString& name)
{
    m_masterName = name;

    if (m_masterName.isEmpty()) {
        setWindowTitle(tr("Veyon Chat - Master"));
    } else {
        setWindowTitle(tr("Veyon Chat - Master (%1)").arg(m_masterName));
    }

    if (m_trayIcon) {
        m_trayIcon->setToolTip(windowTitle());
    }
}

void ChatMasterWidget::closeEvent(QCloseEvent* event)
{
    saveSettings();

    if (m_trayIcon && m_trayIcon->isVisible()) {
        hide();
        event->ignore();
        m_trayIcon->showMessage(tr("Veyon Chat"), tr("Application was minimized to tray."),
                                QSystemTrayIcon::Information, 2000);
        return;
    }

    QWidget::closeEvent(event);
}

void ChatMasterWidget::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange) {
        updateClientList();
        updateChatDisplay();
    }

    QWidget::changeEvent(event);
}

void ChatMasterWidget::onClientSelectionChanged()
{
    const QString selectedId = getSelectedClientId();
    if (selectedId == m_currentClientId) {
        return;
    }

    m_currentClientId = selectedId;
    if (auto* session = getCurrentSession()) {
        session->markAllAsRead();
        if (m_statusLabel) {
            m_statusLabel->setText(tr("%1 (%2)")
                                       .arg(session->clientName(), statusToDisplayString(session->status())));
        }
    } else if (m_statusLabel) {
        m_statusLabel->setText(tr("Ready"));
    }

    updateClientList();
    updateChatDisplay();
    onMessageInputChanged();
}

void ChatMasterWidget::onSendButtonClicked()
{
    const QString clientId = m_currentClientId;
    if (clientId.isEmpty() || !m_messageInput) {
        return;
    }

    const QString content = m_messageInput->text().trimmed();
    if (content.isEmpty()) {
        return;
    }

    const auto priority = static_cast<ChatMessage::Priority>(m_priorityCombo->currentIndex());
    ChatMessage message(QStringLiteral("master"), clientId, content, priority);

    if (auto it = m_sessions.find(clientId); it != m_sessions.end()) {
        it.value().addMessage(message);
        it.value().markAllAsRead();
    } else {
        ChatSession session(clientId);
        session.addMessage(message);
        session.markAllAsRead();
        m_sessions.insert(clientId, session);
    }

    emit sendMessage(message);

    addMessageToDisplay(message);
    m_messageInput->clear();
    onMessageInputChanged();
    updateClientList();
}

void ChatMasterWidget::onClearChatClicked()
{
    if (auto* session = getCurrentSession()) {
        session->clearHistory();
        updateChatDisplay();
        updateClientList();
        emit clearClientChat(session->clientId());
    }
}

void ChatMasterWidget::onGlobalBroadcastClicked()
{
    bool ok = false;
    const QString text = QInputDialog::getMultiLineText(this, tr("Global Broadcast"),
                                                        tr("Enter the message to send to all clients:"),
                                                        QString(), &ok).trimmed();
    if (!ok || text.isEmpty()) {
        return;
    }

    const auto priority = static_cast<ChatMessage::Priority>(m_priorityCombo->currentIndex());
    emit sendGlobalMessage(text, priority);

    if (m_statusLabel) {
        m_statusLabel->setText(tr("Broadcast sent"));
    }
}

void ChatMasterWidget::onMessageInputChanged()
{
    if (!m_messageInput) {
        return;
    }

    const bool hasText = !m_messageInput->text().trimmed().isEmpty();
    if (m_sendButton) {
        m_sendButton->setEnabled(hasText && !m_currentClientId.isEmpty());
    }

    if (hasText) {
        if (m_statusLabel) {
            m_statusLabel->setText(tr("Typing…"));
        }
        m_typingTimer->start();
    } else {
        if (m_statusLabel) {
            m_statusLabel->setText(tr("Ready"));
        }
        m_typingTimer->stop();
    }
}

void ChatMasterWidget::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
        if (isVisible() && isActiveWindow()) {
            hide();
        } else {
            show();
            raise();
            activateWindow();
        }
    }
}

void ChatMasterWidget::onTypingTimer()
{
    if (m_statusLabel) {
        m_statusLabel->setText(tr("Ready"));
    }
}

void ChatMasterWidget::playNotificationSound()
{
    if (!m_soundEnabled || !m_notificationSound) {
        return;
    }

    if (m_notificationSound->status() == QSoundEffect::Error) {
        m_notificationSound->stop();
        m_notificationSound->setSource(QUrl(QStringLiteral("qrc:/chat/sounds/notification.wav")));
    }

    if (m_notificationSound->source().isEmpty()) {
        m_notificationSound->setSource(QUrl(QStringLiteral("qrc:/chat/sounds/notification.wav")));
    }

    m_notificationSound->stop();
    m_notificationSound->play();
}

void ChatMasterWidget::setupUI()
{
    Ui::ChatMasterWidget ui;
    ui.setupUi(this);

    m_splitter = ui.splitter;
    m_clientList = ui.clientList;
    m_chatDisplay = ui.chatDisplay;
    m_messageInput = ui.messageInput;
    m_sendButton = ui.sendButton;
    m_clearButton = ui.clearButton;
    m_globalButton = ui.globalButton;
    m_priorityCombo = ui.priorityCombo;
    m_quickReplies = ui.quickReplies;
    m_statusLabel = ui.statusLabel;

    if (m_chatDisplay) {
        m_chatDisplay->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    }

    if (m_sendButton) {
        m_sendButton->setEnabled(false);
        connect(m_sendButton, &QPushButton::clicked, this, &ChatMasterWidget::onSendButtonClicked);
    }

    if (m_clearButton) {
        connect(m_clearButton, &QPushButton::clicked, this, &ChatMasterWidget::onClearChatClicked);
    }

    if (m_globalButton) {
        connect(m_globalButton, &QPushButton::clicked, this, &ChatMasterWidget::onGlobalBroadcastClicked);
    }

    if (m_clientList) {
        connect(m_clientList, &QListWidget::currentItemChanged, this, [this](QListWidgetItem*, QListWidgetItem*) {
            onClientSelectionChanged();
        });
    }

    if (m_messageInput) {
        connect(m_messageInput, &QLineEdit::textChanged, this, &ChatMasterWidget::onMessageInputChanged);
        connect(m_messageInput, &QLineEdit::returnPressed, this, &ChatMasterWidget::onSendButtonClicked);
    }

    if (m_notificationSound) {
        m_notificationSound->setSource(QUrl(QStringLiteral("qrc:/chat/sounds/notification.wav")));
        m_notificationSound->setLoopCount(1);
        m_notificationSound->setVolume(0.35f);
    }
}

void ChatMasterWidget::setupTrayIcon()
{
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        return;
    }

    if (!m_trayIcon) {
        m_trayIcon = new QSystemTrayIcon(QIcon(QStringLiteral(":/chat/icons/chat.png")), this);
    }

    auto* menu = new QMenu(this);
    auto* showAction = menu->addAction(tr("Show"));
    auto* hideAction = menu->addAction(tr("Hide"));
    menu->addSeparator();
    auto* quitAction = menu->addAction(tr("Quit"));

    connect(showAction, &QAction::triggered, this, [this]() {
        show();
        raise();
        activateWindow();
    });

    connect(hideAction, &QAction::triggered, this, &QWidget::hide);
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

    m_trayIcon->setContextMenu(menu);
    m_trayIcon->setToolTip(windowTitle());
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &ChatMasterWidget::onTrayIconActivated);
    m_trayIcon->show();
}

void ChatMasterWidget::setupShortcuts()
{
    if (!m_f10Shortcut) {
        m_f10Shortcut = new QShortcut(QKeySequence(Qt::Key_F10), this);
    }
    connect(m_f10Shortcut, &QShortcut::activated, this, [this]() {
        if (isVisible()) {
            hide();
        } else {
            show();
            raise();
            activateWindow();
        }
    });

    if (m_messageInput) {
        if (!m_sendShortcut) {
            m_sendShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Return), m_messageInput);
        }
        connect(m_sendShortcut, &QShortcut::activated, this, &ChatMasterWidget::onSendButtonClicked);
    }
}

void ChatMasterWidget::setupQuickReplies()
{
    if (!m_quickReplies || !m_messageInput) {
        return;
    }

    connect(m_quickReplies, QOverload<int>::of(&QComboBox::activated), this, [this](int index) {
        if (index <= 0) {
            return;
        }

        const QString reply = m_quickReplies->itemText(index);
        m_messageInput->setText(reply);
        m_messageInput->setFocus(Qt::TabFocusReason);
        m_messageInput->selectAll();
        m_quickReplies->setCurrentIndex(0);
    });
}

void ChatMasterWidget::loadSettings()
{
    QSettings settings(QStringLiteral("Manus AI"), QStringLiteral("ChatMasterWidget"));
    restoreGeometry(settings.value(QStringLiteral("geometry")).toByteArray());
    if (m_splitter) {
        m_splitter->restoreState(settings.value(QStringLiteral("splitterState")).toByteArray());
    }

    m_soundEnabled = settings.value(QStringLiteral("soundEnabled"), true).toBool();
    if (m_notificationSound) {
        m_notificationSound->setMuted(!m_soundEnabled);
    }
}

void ChatMasterWidget::saveSettings()
{
    QSettings settings(QStringLiteral("Manus AI"), QStringLiteral("ChatMasterWidget"));
    settings.setValue(QStringLiteral("geometry"), saveGeometry());
    if (m_splitter) {
        settings.setValue(QStringLiteral("splitterState"), m_splitter->saveState());
    }
    settings.setValue(QStringLiteral("soundEnabled"), m_soundEnabled);
}

void ChatMasterWidget::updateClientList()
{
    if (!m_clientList) {
        return;
    }

    const QString previousId = getSelectedClientId();
    m_clientList->blockSignals(true);
    m_clientList->clear();

    for (auto it = m_sessions.cbegin(); it != m_sessions.cend(); ++it) {
        const ChatSession& session = it.value();
        QString label = session.clientName();
        if (session.hasUnreadMessages()) {
            label += tr(" (%1 unread)").arg(session.unreadCount());
        }

        auto* item = new QListWidgetItem(QIcon(statusIconPath(session.status())), label, m_clientList);
        item->setData(Qt::UserRole, session.clientId());
        item->setToolTip(statusToDisplayString(session.status()));
        if (session.clientId() == m_currentClientId) {
            m_clientList->setCurrentItem(item);
        }
    }

    if (!m_currentClientId.isEmpty() && previousId != m_currentClientId) {
        for (int i = 0; i < m_clientList->count(); ++i) {
            auto* item = m_clientList->item(i);
            if (item && item->data(Qt::UserRole).toString() == m_currentClientId) {
                m_clientList->setCurrentItem(item);
                break;
            }
        }
    }

    m_clientList->blockSignals(false);
}

void ChatMasterWidget::updateChatDisplay()
{
    if (!m_chatDisplay) {
        return;
    }

    m_chatDisplay->clear();

    const ChatSession* session = getCurrentSession();
    if (!session) {
        return;
    }

    const auto history = session->history();
    for (const ChatMessage& message : history) {
        addMessageToDisplay(message);
    }
}

void ChatMasterWidget::addMessageToDisplay(const ChatMessage& message)
{
    if (!m_chatDisplay) {
        return;
    }

    m_chatDisplay->append(formatMessage(message));
    scrollToBottom();
}

void ChatMasterWidget::scrollToBottom()
{
    if (!m_chatDisplay) {
        return;
    }

    if (auto* scrollBar = m_chatDisplay->verticalScrollBar()) {
        scrollBar->setValue(scrollBar->maximum());
    }
}

QString ChatMasterWidget::formatMessage(const ChatMessage& message) const
{
    const QString timestamp = message.formattedTimestamp();
    const QString priority = message.priorityString();
    QString sender = message.senderId();
    if (sender == QStringLiteral("master")) {
        sender = tr("You");
    }

    return tr("[%1] (%2) %3: %4 [%5]")
        .arg(timestamp, priority, sender, message.content(), message.statusString());
}

QString ChatMasterWidget::getSelectedClientId() const
{
    if (!m_clientList) {
        return QString();
    }

    if (auto* item = m_clientList->currentItem()) {
        return item->data(Qt::UserRole).toString();
    }

    return QString();
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
