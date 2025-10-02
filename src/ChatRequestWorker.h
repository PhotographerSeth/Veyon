#pragma once

#include <QObject>
#include <QUdpSocket>
#include <QAbstractNativeEventFilter>

class HotkeyFilter : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    explicit HotkeyFilter(QObject* parent = nullptr) : QObject(parent) {}

    bool nativeEventFilter(const QByteArray& eventType, void* message, long* result) override;

signals:
    void f10Pressed();
};

class ChatRequestWorker : public QObject
{
    Q_OBJECT

public:
    explicit ChatRequestWorker(QObject* parent = nullptr);
    ~ChatRequestWorker() override;

public slots:
    void sendRequest();

private:
    HotkeyFilter* m_filter;
};
