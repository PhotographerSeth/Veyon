#pragma once

#include <QHash>
#include <QString>
#include <QVariant>

class FeatureMessage
{
public:
    FeatureMessage(const QString& uid = QString(), int command = 0) :
        m_featureUid(uid),
        m_command(command)
    {
    }

    void addArgument(const QString& key, const QVariant& value)
    {
        m_arguments.insert(key, value);
    }

    QVariant argument(const QString& key) const
    {
        return m_arguments.value(key);
    }

    QString featureUid() const
    {
        return m_featureUid;
    }

    int command() const
    {
        return m_command;
    }

private:
    QString m_featureUid;
    int m_command;
    QHash<QString, QVariant> m_arguments;
};

