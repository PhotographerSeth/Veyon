/*
 * ChatFeaturePlugin.h - declaration of ChatFeaturePlugin class
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

#include "FeatureProviderInterface.h"
#include "Feature.h"
#include "PluginInterface.h"
#include "ChatMasterWidget.h"
#include "ChatServiceClient.h"
#include "ComputerControlInterface.h"

class VeyonWorkerInterface;
class ChatSignalListener;

class ChatFeaturePlugin : public QObject, FeatureProviderInterface, PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "io.veyon.Veyon.Plugins.ChatFeaturePlugin")
    Q_INTERFACES(FeatureProviderInterface PluginInterface)

public:
    ChatFeaturePlugin(QObject* parent = nullptr);
    ~ChatFeaturePlugin() override = default;

    // PluginInterface
    Plugin::Uid uid() const override;
    QVersionNumber version() const override;
    QString name() const override;
    QString description() const override;
    QString vendor() const override;
    QString copyright() const override;
    QString shortName() const override;
    void upgrade(const QVersionNumber& oldVersion) override;

    // FeatureProviderInterface
    const FeatureList& featureList() const override;
    bool controlFeature(Feature::Uid featureUid, Operation operation,
                       const QVariantMap& arguments,
                       const ComputerControlInterfaceList& computerControlInterfaces) override;
    bool handleFeatureMessage(VeyonServerInterface& server,
                             const MessageContext& messageContext,
                             const FeatureMessage& message) override;
    bool handleFeatureMessage(VeyonWorkerInterface& worker, const FeatureMessage& message) override;

    static Feature::Uid chatFeatureUid() { return QStringLiteral("a1b2c3d4-e5f6-7890-abcd-ef1234567890"); }

private slots:
    void openChatWindow();

private:
    enum Commands {
        OpenChatWindow,
        SendMessage,
        ReceiveMessage,
        UpdateStatus,
        ClearChat,
        GlobalBroadcast
    };

    const Feature m_chatFeature;
    FeatureList m_features;
    
    ChatMasterWidget* m_masterWidget;
    ChatServiceClient* m_serviceClient;
    VeyonWorkerInterface* m_workerInterface;
    ChatSignalListener* m_signalListener;
    ComputerControlInterfaceList m_activeControlInterfaces;

    void initializeFeatures();
    void setupKeyboardShortcuts();
    void openOrFocusChatForHost(const QString& hostName);
};
