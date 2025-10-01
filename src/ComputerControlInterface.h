#pragma once

#include <QList>
#include <QString>

class FeatureMessage;

class Computer
{
public:
    QString hostAddress() const
    {
        return QString();
    }
};

class ComputerControlInterface
{
public:
    Computer computer() const
    {
        return Computer();
    }

    void sendFeatureMessage(const FeatureMessage& message, bool synchronous)
    {
        (void)message;
        (void)synchronous;
    }
};

using ComputerControlInterfaceList = QList<ComputerControlInterface*>;

