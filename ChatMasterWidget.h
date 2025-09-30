/*
 * ChatMasterWidget.h - declaration of ChatMasterWidget class
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
#include "ChatSession.h"
#include "ChatMessage.h"

QT_BEGIN_NAMESPACE
class QListWidget;
class QListWidgetItem;
class QTextEdit;
class QLineEdit;
class QPushButton;
class QComboBox;
class QLabel;
class QSplitter;
QT_END_NAMESPACE

class ChatMasterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChatMasterWidget(QWidget* parent = nullptr);
    ~ChatMasterWidget() override;

    // Session management
    void addClient(const QString& clientId, const QString& clientName);
    void removeClient(const QString& clientId);
    void updateClientStatus(const QString& clientId, ChatSession::ClientStatus status);
    
    // Message handling
    void receiveMessage(const ChatMessage& message);
    void updateMessageStatus(const QString& messageId, ChatMessage::Status status);
    
    // Settings
    void setMasterName(const QString& name);
    QString masterName() const { return m_masterName; }

signals:
    void sendMessage(const ChatMessage& message);
    void sendGlobalMessage(const QString& content, ChatMessage::Priority priority);
    void clearClientChat(const QString& clientId);

protected:
    void closeEvent(QCloseEvent* event) override;
    void changeEvent(QEvent* event) override;

private slots:
    void onClientSelectionChanged();
    void onSendButtonClicked();
    void onClearChatClicked();
    void onGlobalBroadcastClicked();
    void onMessageInputChanged();
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onTypingTimer();
    void playNotificationSound();

private:
    void setupUI();
    void setupTrayIcon();
    void setupShortcuts();
    void setupQuickReplies();
    void loadSettings();
    void saveSettings();
    
    void updateClientList();
    void updateChatDisplay();
    void addMessageToDisplay(const ChatMessage& message);
    void scrollToBottom();
    
    QString formatMessage(const ChatMessage& message) const;
    QString getSelectedClientId() const;
    ChatSession* getCurrentSession();
    
    // UI components
    QSplitter* m_splitter;
    QListWidget* m_clientList;
    QTextEdit* m_chatDisplay;
    QLineEdit* m_messageInput;
    QPushButton* m_sendButton;
    QPushButton* m_clearButton;
    QPushButton* m_globalButton;
    QComboBox* m_priorityCombo;
    QComboBox* m_quickReplies;
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
    QMap<QString, ChatSession> m_sessions;
    QString m_masterName;
    QString m_currentClientId;
    bool m_soundEnabled;
};
