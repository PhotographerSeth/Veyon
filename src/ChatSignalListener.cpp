#include "ChatSignalListener.h"

#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>

namespace {
static constexpr quint16 kChatSignalPort = 29665;
}

ChatSignalListener::ChatSignalListener(QObject* parent)
    : QObject(parent)
    , m_socket(new QUdpSocket(this))
{
    m_socket->bind(QHostAddress::AnyIPv4, kChatSignalPort,
                   QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    connect(m_socket, &QUdpSocket::readyRead, this, &ChatSignalListener::onReadyRead);
}

void ChatSignalListener::onReadyRead()
{
    while (m_socket->hasPendingDatagrams()) {
        QByteArray data;
        data.resize(int(m_socket->pendingDatagramSize()));
        QHostAddress peerAddress;
        quint16 peerPort = 0;
        m_socket->readDatagram(data.data(), data.size(), &peerAddress, &peerPort);
        const auto doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            continue;
        }

        const auto object = doc.object();
        if (object.value(QStringLiteral("type")) != QStringLiteral("chat_request")) {
            continue;
        }

        const QString hostName = object.value(QStringLiteral("host")).toString();
        if (hostName.isEmpty()) {
            emit requestFromHost(peerAddress.toString());
        } else {
            emit requestFromHost(hostName);
        }
    }
}
