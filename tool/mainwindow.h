#include <QMainWindow>
#include "infomodel.h"

class QDBusPendingCallWatcher;
class InfoModel;
class QNetworkAccessManager;
class QPushButton;
namespace Ui
{
class Tool;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow (QWidget* parent = 0, Qt::WindowFlags flags = 0);
    virtual ~MainWindow();

    void loadAccount();
    bool loadFileList();

private slots:
    void accountsFinished(QDBusPendingCallWatcher* watcher);
    void filesFinished(QDBusPendingCallWatcher* watcher);
    void addAccountButtonClicked();
    void deleteAccountButtonClicked();
    void fileListActivated();
    void listButtonClicked();
    void fileListFinished(const QCloud::InfoList& info);
private:
    QWidget* m_widget;
    Ui::Tool* m_ui;
    InfoModel* m_accountModel;
    InfoModel* m_fileModel;
    QPushButton* m_addAccountButton;
    QPushButton* m_deleteAccountButton;
    QPushButton* m_listButton;
    QString currentDir;
};
