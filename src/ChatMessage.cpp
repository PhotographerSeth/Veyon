/*
 * ChatMessage.cpp - implementation of ChatMessage class
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

#include "ChatMessage.h"
#include <QUuid>
#include <QJsonObject>

ChatMessage::ChatMessage() :
    m_priority(Priority::Normal),
    m_status(Status::Sent),
    m_timestamp(QDateTime::currentDateTime())
{
    generateMessageId();
}

ChatMessage::ChatMessage(const QString& senderId, const QString& receiverId, 
                         const QString& content, Priority priority) :
    m_senderId(senderId),
    m_receiverId(receiverId),
    m_content(content),
    m_priority(priority),
    m_status(Status::Sent),
    m_timestamp(QDateTime::currentDateTime())
{
    generateMessageId();
}

void ChatMessage::generateMessageId()
{
    m_messageId = QUuid::createUuid().toString(QUuid::WithoutBraces);
}

QJsonObject ChatMessage::toJson() const
{
    QJsonObject json;
    json["messageId"] = m_messageId;
    json["senderId"] = m_senderId;
    json["receiverId"] = m_receiverId;
    json["content"] = m_content;
    json["timestamp"] = m_timestamp.toMSecsSinceEpoch();
    json["priority"] = static_cast<int>(m_priority);
    json["status"] = static_cast<int>(m_status);
    return json;
}

ChatMessage ChatMessage::fromJson(const QJsonObject& json)
{
    ChatMessage message;
    message.m_messageId = json["messageId"].toString();
    message.m_senderId = json["senderId"].toString();
    message.m_receiverId = json["receiverId"].toString();
    message.m_content = json["content"].toString();
    message.m_timestamp = QDateTime::fromMSecsSinceEpoch(json["timestamp"].toVariant().toLongLong());
    message.m_priority = static_cast<Priority>(json["priority"].toInt());
    message.m_status = static_cast<Status>(json["status"].toInt());
    return message;
}

QString ChatMessage::priorityString() const
{
    switch (m_priority) {
        case Priority::Normal: return "Normal";
        case Priority::Urgent: return "Urgent";
        case Priority::Announcement: return "Announcement";
    }
    return "Normal";
}

QString ChatMessage::statusString() const
{
    switch (m_status) {
        case Status::Sent: return "Sent";
        case Status::Delivered: return "Delivered";
        case Status::Read: return "Read";
    }
    return "Sent";
}

QString ChatMessage::formattedTimestamp() const
{
    return m_timestamp.toString("hh:mm:ss");
}
