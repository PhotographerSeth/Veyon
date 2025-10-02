#include "ChatRequestWorker.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QHostAddress>
#include <QHostInfo>
#include <QJsonDocument>
#include <QJsonObject>

#ifdef _WIN32
#include <windows.h>
#endif

namespace {
static constexpr quint16 kChatSignalPort = 29665;
}

bool HotkeyFilter::nativeEventFilter(const QByteArray& eventType, void* message, long*)
{
#ifdef _WIN32
    if (eventType == "windows_generic_MSG") {
        MSG* msg = static_cast<MSG*>(message);
        if (msg->message == WM_HOTKEY && msg->wParam == 1) {
            emit f10Pressed();
        }
    }
#else
    Q_UNUSED(eventType);
    Q_UNUSED(message);
#endif
    return false;
}

ChatRequestWorker::ChatRequestWorker(QObject* parent)
    : QObject(parent)
    , m_filter(new HotkeyFilter(this))
{
#ifdef _WIN32
    RegisterHotKey(nullptr, 1, 0x4000 /*MOD_NOREPEAT*/, VK_F10);
#endif

    if (qApp) {
        qApp->installNativeEventFilter(m_filter);
    }
    connect(m_filter, &HotkeyFilter::f10Pressed, this, &ChatRequestWorker::sendRequest);
}

ChatRequestWorker::~ChatRequestWorker()
{
    if (m_filter && qApp) {
        qApp->removeNativeEventFilter(m_filter);
    }
#ifdef _WIN32
    UnregisterHotKey(nullptr, 1);
#endif
}

void ChatRequestWorker::sendRequest()
{
    QUdpSocket socket;

    QString user = QString::fromLocal8Bit(qgetenv("USERNAME"));
    if (user.isEmpty()) {
        user = QString::fromLocal8Bit(qgetenv("USER"));
    }

    const QJsonObject payload{
        {QStringLiteral("type"), QStringLiteral("chat_request")},
        {QStringLiteral("host"), QHostInfo::localHostName()},
        {QStringLiteral("user"), user},
        {QStringLiteral("ts"), QDateTime::currentDateTimeUtc().toString(Qt::ISODate)}
    };

    const QByteArray data = QJsonDocument(payload).toJson(QJsonDocument::Compact);
    socket.writeDatagram(data, QHostAddress::Broadcast, kChatSignalPort);
}
