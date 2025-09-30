/*
 * ChatServiceClient.cpp - implementation of ChatServiceClient class
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

#include "ChatServiceClient.h"
#include <QHostInfo>
#include <QApplication>

ChatServiceClient::ChatServiceClient(QObject* parent) :
    QObject(parent),
    m_clientWidget(nullptr)
{
    initializeClient();
}

ChatServiceClient::~ChatServiceClient()
{
    if (m_clientWidget) {
        m_clientWidget->deleteLater();
    }
}

void ChatServiceClient::receiveMessage(const ChatMessage& message)
{
    if (!m_clientWidget) {
        initializeClient();
    }
    
    m_clientWidget->receiveMessage(message);
    
    // Show the chat window if it's not visible and we received a message
    if (!m_clientWidget->isVisible()) {
        showChatWindow();
    }
}

void ChatServiceClient::clearChat()
{
    if (m_clientWidget) {
        m_clientWidget->clearChat();
    }
}

void ChatServiceClient::showChatWindow()
{
    if (!m_clientWidget) {
        initializeClient();
    }
    
    m_clientWidget->show();
    m_clientWidget->raise();
    m_clientWidget->activateWindow();
}

void ChatServiceClient::hideChatWindow()
{
    if (m_clientWidget) {
        m_clientWidget->hide();
    }
}

void ChatServiceClient::onClientMessageSent(const ChatMessage& message)
{
    emit sendMessage(message);
}

void ChatServiceClient::onClientStatusChanged(ChatSession::ClientStatus status)
{
    emit statusChanged(status);
}

void ChatServiceClient::initializeClient()
{
    if (m_clientWidget) {
        return;
    }
    
    m_clientId = getClientId();
    m_clientWidget = new ChatClientWidget();
    
    // Connect signals
    connect(m_clientWidget, &ChatClientWidget::sendMessage,
            this, &ChatServiceClient::onClientMessageSent);
    connect(m_clientWidget, &ChatClientWidget::statusChanged,
            this, &ChatServiceClient::onClientStatusChanged);
}

QString ChatServiceClient::getClientId() const
{
    return QHostInfo::localHostName();
}
