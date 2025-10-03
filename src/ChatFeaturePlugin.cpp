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
#include <QApplication>
#include <QShortcut>
#include <QKeySequence>

#include "ChatSignalListener.h"

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
    m_serviceClient(nullptr),
    m_workerInterface(nullptr),
    m_signalListener(new ChatSignalListener(this))
{
    initializeFeatures();
    setupKeyboardShortcuts();

    connect(m_signalListener, &ChatSignalListener::requestFromHost, this, [this](const QString& host) {
        openOrFocusChatForHost(host);
    });
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
            m_activeControlInterfaces = computerControlInterfaces;
            openChatWindow();
            return true;

        case Operation::Stop:
            m_activeControlInterfaces.clear();
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
    if (message.featureUid() != chatFeatureUid()) {
        return false;
    }

    // Initialize service client if not already done
    if (!m_serviceClient) {
        m_serviceClient = new ChatServiceClient(this);
        connect(m_serviceClient, &ChatServiceClient::sendMessage,
                this, [this](const ChatMessage& chatMessage) {
                    if (!m_workerInterface) {
                        return;
                    }

                    FeatureMessage featureMessage(chatFeatureUid(), ReceiveMessage);
                    featureMessage.addArgument(QStringLiteral("message"), chatMessage.toJson());
                    m_workerInterface->sendFeatureMessage(featureMessage);
                });

        connect(m_serviceClient, &ChatServiceClient::statusChanged,
                this, [this](ChatSession::ClientStatus status) {
                    if (!m_workerInterface) {
                        return;
                    }

                    FeatureMessage featureMessage(chatFeatureUid(), UpdateStatus);
                    featureMessage.addArgument(QStringLiteral("clientId"), m_serviceClient->clientId());
                    featureMessage.addArgument(QStringLiteral("status"), static_cast<int>(status));
                    m_workerInterface->sendFeatureMessage(featureMessage);
                });
    }

    m_workerInterface = &worker;

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
                    for (auto* controlInterface : m_activeControlInterfaces) {
                        if (!controlInterface) {
                            continue;
                        }

                        FeatureMessage featureMessage(chatFeatureUid(), SendMessage);
                        featureMessage.addArgument(QStringLiteral("message"), message.toJson());
                        controlInterface->sendFeatureMessage(featureMessage, false);
                    }
                });

        connect(m_masterWidget, &ChatMasterWidget::sendGlobalMessage,
                this, [this](const QString& content, ChatMessage::Priority priority) {
                    ChatMessage broadcast(QStringLiteral("master"), QStringLiteral("all"), content, priority);

                    for (auto* controlInterface : m_activeControlInterfaces) {
                        if (!controlInterface) {
                            continue;
                        }

                        FeatureMessage featureMessage(chatFeatureUid(), GlobalBroadcast);
                        featureMessage.addArgument(QStringLiteral("message"), broadcast.toJson());
                        controlInterface->sendFeatureMessage(featureMessage, false);
                    }
                });

        connect(m_masterWidget, &ChatMasterWidget::clearClientChat,
                this, [this](const QString& clientId) {
                    for (auto* controlInterface : m_activeControlInterfaces) {
                        if (!controlInterface) {
                            continue;
                        }

                        if (!clientId.isEmpty() && controlInterface->computer().hostAddress() != clientId) {
                            continue;
                        }

                        FeatureMessage featureMessage(chatFeatureUid(), ClearChat);
                        featureMessage.addArgument(QStringLiteral("clientId"), clientId);
                        controlInterface->sendFeatureMessage(featureMessage, false);
                    }
                });
    }

    m_masterWidget->show();
    m_masterWidget->raise();
    m_masterWidget->activateWindow();
}

void ChatFeaturePlugin::openOrFocusChatForHost(const QString& hostName)
{
    openChatWindow();

    if (!m_masterWidget) {
        return;
    }

    QString targetId = hostName;

    for (ComputerControlInterface* controlInterface : m_activeControlInterfaces) {
        if (!controlInterface) {
            continue;
        }

        const QString hostAddress = controlInterface->computer().hostAddress();
        if (!hostAddress.isEmpty() && hostAddress.compare(hostName, Qt::CaseInsensitive) == 0) {
            targetId = hostAddress;
            break;
        }
    }

    if (!targetId.isEmpty()) {
        m_masterWidget->focusClient(targetId);
    }
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
