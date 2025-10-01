/*
 * ChatSession.h - declaration of ChatSession class
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

#include "ChatMessage.h"
#include <QList>
#include <QString>
#include <QDateTime>

class ChatSession
{
public:
    enum class ClientStatus
    {
        Online,
        Away,
        Typing
    };

    ChatSession();
    explicit ChatSession(const QString& clientId);
    
    // Getters
    QString clientId() const { return m_clientId; }
    QString clientName() const { return m_clientName; }
    ClientStatus status() const { return m_status; }
    QList<ChatMessage> history() const { return m_history; }
    QDateTime lastActivity() const { return m_lastActivity; }
    int unreadCount() const { return m_unreadCount; }
    
    // Setters
    void setClientName(const QString& name) { m_clientName = name; }
    void setStatus(ClientStatus status);
    
    // Message management
    void addMessage(const ChatMessage& message);
    void clearHistory();
    void markAllAsRead();
    bool updateMessageStatus(const QString& messageId, ChatMessage::Status status);
    
    // Utility
    QString statusString() const;
    bool hasUnreadMessages() const { return m_unreadCount > 0; }

private:
    QString m_clientId;
    QString m_clientName;
    ClientStatus m_status;
    QList<ChatMessage> m_history;
    QDateTime m_lastActivity;
    int m_unreadCount;
    
    void updateLastActivity();
};
