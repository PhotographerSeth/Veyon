/*
 * ChatClientWidget.h - declaration of ChatClientWidget class
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

#pragma once

#include <QWidget>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QShortcut>
#include <QSoundEffect>
#include "ChatMessage.h"
#include "ChatSession.h"

QT_BEGIN_NAMESPACE
class QTextEdit;
class QLineEdit;
class QPushButton;
class QLabel;
QT_END_NAMESPACE

class ChatClientWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChatClientWidget(QWidget* parent = nullptr);
    ~ChatClientWidget() override;

    // Message handling
    void receiveMessage(const ChatMessage& message);
    void clearChat();

    // Settings
    void setMasterName(const QString& name);
    void setClientId(const QString& clientId);
    QString masterName() const { return m_masterName; }

signals:
    void sendMessage(const ChatMessage& message);
    void statusChanged(ChatSession::ClientStatus status);

protected:
    void closeEvent(QCloseEvent* event) override;
    void changeEvent(QEvent* event) override;

private slots:
    void onSendButtonClicked();
    void onMessageInputChanged();
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onTypingTimer();
    void playNotificationSound();

private:
    void setupUI();
    void setupTrayIcon();
    void setupShortcuts();
    void loadSettings();
    void saveSettings();
    
    void addMessageToDisplay(const ChatMessage& message);
    void scrollToBottom();
    void updateWindowTitle();
    
    QString formatMessage(const ChatMessage& message) const;
    
    // UI components
    QTextEdit* m_chatDisplay;
    QLineEdit* m_messageInput;
    QPushButton* m_sendButton;
    QLabel* m_statusLabel;
    
    // System tray
    QSystemTrayIcon* m_trayIcon;
    
    // Shortcuts
    QShortcut* m_f10Shortcut;
    QShortcut* m_sendShortcut;
    
    // Timers
    QTimer* m_typingTimer;
    
    // Sound
    QSoundEffect* m_notificationSound;
    
    // Data
    QString m_masterName;
    QString m_clientId;
    bool m_soundEnabled;
    int m_unreadCount;
};
