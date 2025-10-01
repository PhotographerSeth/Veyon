/*
 * ChatClientWidget.cpp - implementation of ChatClientWidget class
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

#include "ChatClientWidget.h"

#include "ChatMessage.h"
#include "ChatSession.h"
#include "ui_ChatClientWidget.h"

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDateTime>
#include <QEvent>
#include <QHostInfo>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QScrollBar>
#include <QSettings>
#include <QShortcut>
#include <QSoundEffect>
#include <QSystemTrayIcon>
#include <QTextEdit>
#include <QTextOption>
#include <QTimer>
#include <QUrl>

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
    m_unreadCount(0)
{
    m_clientId = QHostInfo::localHostName();

    setupUI();
    setupTrayIcon();
    setupShortcuts();
    loadSettings();

    connect(m_typingTimer, &QTimer::timeout, this, &ChatClientWidget::onTypingTimer);
    m_typingTimer->setSingleShot(true);
    m_typingTimer->setInterval(1800);

    updateWindowTitle();
}

ChatClientWidget::~ChatClientWidget()
{
    saveSettings();
}

void ChatClientWidget::receiveMessage(const ChatMessage& message)
{
    addMessageToDisplay(message);

    if (!isActiveWindow()) {
        ++m_unreadCount;
        updateWindowTitle();
    }

    if (m_trayIcon && m_trayIcon->isVisible()) {
        m_trayIcon->showMessage(tr("Message from %1").arg(m_masterName.isEmpty() ? tr("Teacher") : m_masterName),
                                message.content(), QSystemTrayIcon::Information, 3000);
    }

    playNotificationSound();
}

void ChatClientWidget::clearChat()
{
    if (m_chatDisplay) {
        m_chatDisplay->clear();
    }
    m_unreadCount = 0;
    updateWindowTitle();
}

void ChatClientWidget::setMasterName(const QString& name)
{
    m_masterName = name;
    updateWindowTitle();
}

void ChatClientWidget::closeEvent(QCloseEvent* event)
{
    saveSettings();

    if (m_trayIcon && m_trayIcon->isVisible()) {
        hide();
        event->ignore();
        return;
    }

    QWidget::closeEvent(event);
}

void ChatClientWidget::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange) {
        updateWindowTitle();
    }

    QWidget::changeEvent(event);
}

void ChatClientWidget::onSendButtonClicked()
{
    if (!m_messageInput) {
        return;
    }

    const QString content = m_messageInput->text().trimmed();
    if (content.isEmpty()) {
        return;
    }

    ChatMessage message(m_clientId, QStringLiteral("master"), content, ChatMessage::Priority::Normal);
    emit sendMessage(message);

    addMessageToDisplay(message);
    m_messageInput->clear();
    onMessageInputChanged();
    emit statusChanged(ChatSession::ClientStatus::Online);
}

void ChatClientWidget::onMessageInputChanged()
{
    if (!m_messageInput) {
        return;
    }

    const bool hasText = !m_messageInput->text().trimmed().isEmpty();
    if (m_sendButton) {
        m_sendButton->setEnabled(hasText);
    }

    if (hasText) {
        if (m_statusLabel) {
            m_statusLabel->setText(tr("Typing…"));
        }
        emit statusChanged(ChatSession::ClientStatus::Typing);
        m_typingTimer->start();
    } else {
        if (m_statusLabel) {
            m_statusLabel->setText(tr("Ready"));
        }
        emit statusChanged(ChatSession::ClientStatus::Online);
        m_typingTimer->stop();
    }
}

void ChatClientWidget::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
        if (isVisible() && isActiveWindow()) {
            hide();
        } else {
            show();
            raise();
            activateWindow();
            m_unreadCount = 0;
            updateWindowTitle();
        }
    }
}

void ChatClientWidget::onTypingTimer()
{
    if (m_statusLabel) {
        m_statusLabel->setText(tr("Ready"));
    }

    emit statusChanged(ChatSession::ClientStatus::Online);
}

void ChatClientWidget::playNotificationSound()
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

void ChatClientWidget::setupUI()
{
    Ui::ChatClientWidget ui;
    ui.setupUi(this);

    m_chatDisplay = ui.chatDisplay;
    m_messageInput = ui.messageInput;
    m_sendButton = ui.sendButton;
    m_statusLabel = ui.statusLabel;

    if (m_chatDisplay) {
        m_chatDisplay->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    }

    if (m_sendButton) {
        m_sendButton->setEnabled(false);
        connect(m_sendButton, &QPushButton::clicked, this, &ChatClientWidget::onSendButtonClicked);
    }

    if (m_messageInput) {
        connect(m_messageInput, &QLineEdit::textChanged, this, &ChatClientWidget::onMessageInputChanged);
        connect(m_messageInput, &QLineEdit::returnPressed, this, &ChatClientWidget::onSendButtonClicked);
    }

    if (m_notificationSound) {
        m_notificationSound->setSource(QUrl(QStringLiteral("qrc:/chat/sounds/notification.wav")));
        m_notificationSound->setLoopCount(1);
        m_notificationSound->setVolume(0.35f);
    }
}

void ChatClientWidget::setupTrayIcon()
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
        m_unreadCount = 0;
        updateWindowTitle();
    });

    connect(hideAction, &QAction::triggered, this, &QWidget::hide);
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

    m_trayIcon->setContextMenu(menu);
    m_trayIcon->setToolTip(windowTitle());
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &ChatClientWidget::onTrayIconActivated);
    m_trayIcon->show();
}

void ChatClientWidget::setupShortcuts()
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
        connect(m_sendShortcut, &QShortcut::activated, this, &ChatClientWidget::onSendButtonClicked);
    }
}

void ChatClientWidget::loadSettings()
{
    QSettings settings(QStringLiteral("Manus AI"), QStringLiteral("ChatClientWidget"));
    restoreGeometry(settings.value(QStringLiteral("geometry")).toByteArray());
    m_soundEnabled = settings.value(QStringLiteral("soundEnabled"), true).toBool();
    if (m_notificationSound) {
        m_notificationSound->setMuted(!m_soundEnabled);
    }
}

void ChatClientWidget::saveSettings()
{
    QSettings settings(QStringLiteral("Manus AI"), QStringLiteral("ChatClientWidget"));
    settings.setValue(QStringLiteral("geometry"), saveGeometry());
    settings.setValue(QStringLiteral("soundEnabled"), m_soundEnabled);
}

void ChatClientWidget::addMessageToDisplay(const ChatMessage& message)
{
    if (!m_chatDisplay) {
        return;
    }

    QString sender = message.senderId();
    if (sender == m_clientId) {
        sender = tr("You");
    } else if (!m_masterName.isEmpty()) {
        sender = m_masterName;
    } else if (sender == QStringLiteral("master")) {
        sender = tr("Teacher");
    }

    const QString line = tr("[%1] %2: %3")
                             .arg(message.formattedTimestamp(), sender, message.content());
    m_chatDisplay->append(line);
    scrollToBottom();
}

void ChatClientWidget::scrollToBottom()
{
    if (!m_chatDisplay) {
        return;
    }

    if (auto* scrollBar = m_chatDisplay->verticalScrollBar()) {
        scrollBar->setValue(scrollBar->maximum());
    }
}

void ChatClientWidget::updateWindowTitle()
{
    QString title = tr("Veyon Chat - Student");
    if (!m_masterName.isEmpty()) {
        title += tr(" (%1)").arg(m_masterName);
    }
    if (m_unreadCount > 0) {
        title += tr(" - %n unread", nullptr, m_unreadCount);
    }

    setWindowTitle(title);
    if (m_trayIcon) {
        m_trayIcon->setToolTip(title);
    }
}

QString ChatClientWidget::formatMessage(const ChatMessage& message) const
{
    QString sender = message.senderId();
    if (sender == m_clientId) {
        sender = tr("You");
    } else if (sender == QStringLiteral("master")) {
        sender = m_masterName.isEmpty() ? tr("Teacher") : m_masterName;
    }

    return tr("[%1] %2: %3")
        .arg(message.formattedTimestamp(), sender, message.content());
}
