/*
 * ChatMessage.h - declaration of ChatMessage class
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

#include <QString>
#include <QDateTime>
#include <QJsonObject>

class ChatMessage
{
public:
    enum class Priority
    {
        Normal,
        Urgent,
        Announcement
    };

    enum class Status
    {
        Sent,
        Delivered,
        Read
    };

    ChatMessage();
    ChatMessage(const QString& senderId, const QString& receiverId, 
                const QString& content, Priority priority = Priority::Normal);
    
    // Getters
    QString messageId() const { return m_messageId; }
    QString senderId() const { return m_senderId; }
    QString receiverId() const { return m_receiverId; }
    QString content() const { return m_content; }
    QDateTime timestamp() const { return m_timestamp; }
    Priority priority() const { return m_priority; }
    Status status() const { return m_status; }
    
    // Setters
    void setStatus(Status status) { m_status = status; }
    void setContent(const QString& content) { m_content = content; }
    
    // Serialization
    QJsonObject toJson() const;
    static ChatMessage fromJson(const QJsonObject& json);
    
    // Utility
    QString priorityString() const;
    QString statusString() const;
    QString formattedTimestamp() const;

private:
    QString m_messageId;
    QString m_senderId;
    QString m_receiverId;
    QString m_content;
    QDateTime m_timestamp;
    Priority m_priority;
    Status m_status;
    
    void generateMessageId();
};
