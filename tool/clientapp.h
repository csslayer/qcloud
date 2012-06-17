#ifndef QCLOUD_CLIENTAPP_H
#define QCLOUD_CLIENTAPP_H

#include <QApplication>
#include "infomodel.h"

class MainWindow;
namespace QCloud
{
class Client;
}
class ClientApp : public QApplication
{
    Q_OBJECT
public:
    ClientApp (int& argc, char** argv);

    static ClientApp* instance() { return static_cast<ClientApp*>(qApp); }

    QCloud::Client* client();

private slots:
    void accountUpdated();

private:
    QCloud::Client* m_client;
    MainWindow* m_mainWindow;
};

#endif // QCLOUD_CLIENTAPP_H