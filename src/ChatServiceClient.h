/*
 * ChatServiceClient.h - declaration of ChatServiceClient class
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

#include <QObject>
#include "ChatMessage.h"
#include "ChatClientWidget.h"
#include "ChatSession.h"

class ChatServiceClient : public QObject
{
    Q_OBJECT

public:
    explicit ChatServiceClient(QObject* parent = nullptr);
    ~ChatServiceClient() override;

    // Message handling
    void receiveMessage(const ChatMessage& message);
    void clearChat();
    
    // Client management
    void showChatWindow();
    void hideChatWindow();

    QString clientId() const { return m_clientId; }

signals:
    void sendMessage(const ChatMessage& message);
    void statusChanged(ChatSession::ClientStatus status);

private slots:
    void onClientMessageSent(const ChatMessage& message);
    void onClientStatusChanged(ChatSession::ClientStatus status);

private:
    void initializeClient();
    QString getClientId() const;
    
    ChatClientWidget* m_clientWidget;
    QString m_clientId;
};
