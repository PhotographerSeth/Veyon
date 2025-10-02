#pragma once

#include <QObject>
#include <QUdpSocket>

class ChatSignalListener : public QObject
{
    Q_OBJECT

public:
    explicit ChatSignalListener(QObject* parent = nullptr);

signals:
    void requestFromHost(const QString& hostName);

private slots:
    void onReadyRead();

private:
    QUdpSocket* m_socket;
};
