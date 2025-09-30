/*
 * ChatFeaturePlugin.cpp - implementation of ChatFeaturePlugin class
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

#include "ChatFeaturePlugin.h"
#include "FeatureMessage.h"
#include "VeyonServerInterface.h"
#include "VeyonWorkerInterface.h"
#include "ComputerControlInterface.h"
#include <QApplication>
#include <QShortcut>
#include <QKeySequence>

ChatFeaturePlugin::ChatFeaturePlugin(QObject* parent) :
    QObject(parent),
    m_chatFeature(chatFeatureUid(),
                  Feature::Flag::Mode | Feature::Flag::Master | Feature::Flag::Service | Feature::Flag::Worker,
                  QStringLiteral("Chat"),
                  QStringLiteral("Open Chat"),
                  QStringLiteral("Chat Active"),
                  QStringLiteral("Enable real-time chat communication between Master and clients"),
                  QStringLiteral(":/chat/icons/chat.png"),
                  QKeySequence(Qt::Key_F10)),
    m_masterWidget(nullptr),
    m_serviceClient(nullptr)
{
    initializeFeatures();
    setupKeyboardShortcuts();
}

Plugin::Uid ChatFeaturePlugin::uid() const
{
    return QStringLiteral("a1b2c3d4-e5f6-7890-abcd-ef1234567890");
}

QVersionNumber ChatFeaturePlugin::version() const
{
    return QVersionNumber(1, 0, 0);
}

QString ChatFeaturePlugin::name() const
{
    return QStringLiteral("Chat");
}

QString ChatFeaturePlugin::description() const
{
    return QStringLiteral("Real-time chat communication for Veyon");
}

QString ChatFeaturePlugin::vendor() const
{
    return QStringLiteral("Manus AI");
}

QString ChatFeaturePlugin::copyright() const
{
    return QStringLiteral("Copyright (c) 2025 Manus AI");
}

QString ChatFeaturePlugin::shortName() const
{
    return QStringLiteral("Chat");
}

void ChatFeaturePlugin::upgrade(const QVersionNumber& oldVersion)
{
    Q_UNUSED(oldVersion)
}

const FeatureList& ChatFeaturePlugin::featureList() const
{
    return m_features;
}

bool ChatFeaturePlugin::controlFeature(Feature::Uid featureUid, Operation operation,
                                      const QVariantMap& arguments,
                                      const ComputerControlInterfaceList& computerControlInterfaces)
{
    if (featureUid != chatFeatureUid()) {
        return false;
    }

    switch (operation) {
        case Operation::Start:
            openChatWindow();
            return true;

        case Operation::Stop:
            if (m_masterWidget) {
                m_masterWidget->close();
            }
            return true;

        default:
            break;
    }

    // Handle specific chat commands
    const auto command = static_cast<Commands>(arguments.value("command").toInt());
    
    switch (command) {
        case SendMessage: {
            const auto message = ChatMessage::fromJson(arguments.value("message").toJsonObject());
            for (const auto& controlInterface : computerControlInterfaces) {
                FeatureMessage featureMessage(featureUid, command);
                featureMessage.addArgument("message", message.toJson());
                controlInterface->sendFeatureMessage(featureMessage, false);
            }
            return true;
        }

        case GlobalBroadcast: {
            const auto content = arguments.value("content").toString();
            const auto priority = static_cast<ChatMessage::Priority>(arguments.value("priority").toInt());
            
            ChatMessage message("master", "all", content, priority);
            
            for (const auto& controlInterface : computerControlInterfaces) {
                FeatureMessage featureMessage(featureUid, command);
                featureMessage.addArgument("message", message.toJson());
                controlInterface->sendFeatureMessage(featureMessage, false);
            }
            return true;
        }

        case ClearChat: {
            const auto clientId = arguments.value("clientId").toString();
            
            for (const auto& controlInterface : computerControlInterfaces) {
                if (controlInterface->computer().hostAddress() == clientId) {
                    FeatureMessage featureMessage(featureUid, command);
                    controlInterface->sendFeatureMessage(featureMessage, false);
                    break;
                }
            }
            return true;
        }

        default:
            break;
    }

    return false;
}

bool ChatFeaturePlugin::handleFeatureMessage(VeyonServerInterface& server,
                                            const MessageContext& messageContext,
                                            const FeatureMessage& message)
{
    Q_UNUSED(server)
    Q_UNUSED(messageContext)

    if (message.featureUid() != chatFeatureUid()) {
        return false;
    }

    const auto command = static_cast<Commands>(message.command());

    switch (command) {
        case ReceiveMessage: {
            // Forward message to master widget if it exists
            if (m_masterWidget) {
                const auto chatMessage = ChatMessage::fromJson(message.argument("message").toJsonObject());
                m_masterWidget->receiveMessage(chatMessage);
            }
            return true;
        }

        case UpdateStatus: {
            // Update client status in master widget
            if (m_masterWidget) {
                const auto clientId = message.argument("clientId").toString();
                const auto status = static_cast<ChatSession::ClientStatus>(message.argument("status").toInt());
                m_masterWidget->updateClientStatus(clientId, status);
            }
            return true;
        }

        default:
            break;
    }

    return false;
}

bool ChatFeaturePlugin::handleFeatureMessage(VeyonWorkerInterface& worker, const FeatureMessage& message)
{
    Q_UNUSED(worker)

    if (message.featureUid() != chatFeatureUid()) {
        return false;
    }

    // Initialize service client if not already done
    if (!m_serviceClient) {
        m_serviceClient = new ChatServiceClient(this);
    }

    const auto command = static_cast<Commands>(message.command());

    switch (command) {
        case SendMessage: {
            const auto chatMessage = ChatMessage::fromJson(message.argument("message").toJsonObject());
            m_serviceClient->receiveMessage(chatMessage);
            return true;
        }

        case GlobalBroadcast: {
            const auto chatMessage = ChatMessage::fromJson(message.argument("message").toJsonObject());
            m_serviceClient->receiveMessage(chatMessage);
            return true;
        }

        case ClearChat: {
            m_serviceClient->clearChat();
            return true;
        }

        default:
            break;
    }

    return false;
}

void ChatFeaturePlugin::openChatWindow()
{
    if (!m_masterWidget) {
        m_masterWidget = new ChatMasterWidget();
        
        // Connect signals for message handling
        connect(m_masterWidget, &ChatMasterWidget::sendMessage,
                this, [this](const ChatMessage& message) {
                    // Send message via feature system
                    QVariantMap arguments;
                    arguments["command"] = SendMessage;
                    arguments["message"] = message.toJson();
                    
                    // This would typically be called through the feature control system
                    // For now, we'll emit a signal or use another mechanism
                });
        
        connect(m_masterWidget, &ChatMasterWidget::sendGlobalMessage,
                this, [this](const QString& content, ChatMessage::Priority priority) {
                    QVariantMap arguments;
                    arguments["command"] = GlobalBroadcast;
                    arguments["content"] = content;
                    arguments["priority"] = static_cast<int>(priority);
                    
                    // Handle global broadcast
                });
        
        connect(m_masterWidget, &ChatMasterWidget::clearClientChat,
                this, [this](const QString& clientId) {
                    QVariantMap arguments;
                    arguments["command"] = ClearChat;
                    arguments["clientId"] = clientId;
                    
                    // Handle clear chat
                });
    }

    m_masterWidget->show();
    m_masterWidget->raise();
    m_masterWidget->activateWindow();
}

void ChatFeaturePlugin::initializeFeatures()
{
    m_features = { m_chatFeature };
}

void ChatFeaturePlugin::setupKeyboardShortcuts()
{
    // Global F10 shortcut will be handled by the individual widgets
    // when they are created and shown
}
