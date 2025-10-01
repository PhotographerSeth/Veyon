#pragma once

#include <QString>
#include <QVariant>

class FeatureMessage
{
public:
    FeatureMessage(const QString& uid = QString(), int command = 0)
    {
        (void)uid;
        (void)command;
    }

    void addArgument(const QString& key, const QVariant& value)
    {
        (void)key;
        (void)value;
    }

    QVariant argument(const QString& key) const
    {
        (void)key;
        return QVariant();
    }

    QString featureUid() const
    {
        return QString();
    }

    int command() const
    {
        return 0;
    }
};

