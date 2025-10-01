/*
 * ChatSession.cpp - implementation of ChatSession class
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

#include "ChatSession.h"

#include <utility>

ChatSession::ChatSession() :
    m_status(ClientStatus::Online),
    m_lastActivity(QDateTime::currentDateTime()),
    m_unreadCount(0)
{
}

ChatSession::ChatSession(const QString& clientId) :
    m_clientId(clientId),
    m_clientName(clientId), // Default to clientId, can be changed later
    m_status(ClientStatus::Online),
    m_lastActivity(QDateTime::currentDateTime()),
    m_unreadCount(0)
{
}

void ChatSession::setStatus(ClientStatus status)
{
    m_status = status;
    updateLastActivity();
}

void ChatSession::addMessage(const ChatMessage& message)
{
    m_history.append(message);
    updateLastActivity();
    
    // If this is an incoming message (not from master), increment unread count
    if (message.senderId() != "master") {
        m_unreadCount++;
    }
}

void ChatSession::clearHistory()
{
    m_history.clear();
    m_unreadCount = 0;
    updateLastActivity();
}

void ChatSession::markAllAsRead()
{
    m_unreadCount = 0;

    // Update status of all messages to read
    for (auto& message : m_history) {
        if (message.status() != ChatMessage::Status::Read) {
            message.setStatus(ChatMessage::Status::Read);
        }
    }
}

bool ChatSession::updateMessageStatus(const QString& messageId, ChatMessage::Status status)
{
    bool updated = false;

    for (auto& message : m_history) {
        if (message.messageId() == messageId) {
            message.setStatus(status);
            updated = true;
            break;
        }
    }

    if (!updated) {
        return false;
    }

    // Recalculate unread count to reflect the new status
    int unread = 0;
    for (const auto& message : std::as_const(m_history)) {
        if (message.senderId() != QStringLiteral("master") &&
            message.status() != ChatMessage::Status::Read) {
            ++unread;
        }
    }

    m_unreadCount = unread;
    updateLastActivity();

    return true;
}

QString ChatSession::statusString() const
{
    switch (m_status) {
        case ClientStatus::Online: return "Online";
        case ClientStatus::Away: return "Away";
        case ClientStatus::Typing: return "Typing...";
    }
    return "Online";
}

void ChatSession::updateLastActivity()
{
    m_lastActivity = QDateTime::currentDateTime();
}
