#ifndef QCLOUD_SERVER_H
#define QCLOUD_SERVER_H
#include <QObject>
#include <QStringList>
#include <QDBusConnection>
#include <QDBusVariant>
#include "qcloud_global.h"

class DaemonAdaptor;
namespace QCloud {
class ServerPrivate;
class QCLOUD_EXPORT Server : public QObject {
    Q_OBJECT
public:
    explicit Server(QObject* parent = 0);
    virtual ~Server();
    virtual int uploadFile(const QString& app_name, const QStringList& file_list) = 0;
    virtual int addAccount (const QString& backend_name, const QString& user_name, const QDBusVariant& account_specific_data) = 0;
    virtual int sync (const QString& app_name) = 0;
private:
    Q_DISABLE_COPY(Server)
    QDBusConnection m_session;
    DaemonAdaptor* m_adaptor;
};
}

#endif