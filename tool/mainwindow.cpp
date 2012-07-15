#include <QDebug>
#include <QFileInfo>

#include "mainwindow.h"
#include "accountdialog.h"
#include "clientapp.h"
#include "infomodel.h"
#include "factory.h"
#include "ibackend.h"
#include "client.h"
#include "appmanager.h"
#include "ui_tool.h"


MainWindow::MainWindow (QWidget* parent, Qt::WindowFlags flags) : QMainWindow (parent, flags)
    , m_widget (new QWidget (this))
    , m_ui (new Ui::Tool)
    , m_accountModel (new InfoModel(this))
    , m_fileModel(new EntryInfoModel(this))
{
    currentDir = "/";

    setCentralWidget (m_widget);
    m_ui->setupUi (m_widget);
    m_ui->accountView->setModel(m_accountModel);
    m_ui->accountView->setSelectionMode(QAbstractItemView::SingleSelection);

    m_addAccountButton = m_ui->addAccountButton;
    m_deleteAccountButton = m_ui->deleteAccountButton;
    m_listButton = m_ui->listButton;

    m_addAccountButton->setIcon (QIcon::fromTheme ("list-add"));
    m_deleteAccountButton->setIcon (QIcon::fromTheme ("list-remove"));

    m_ui->fileView->setModel(m_fileModel);
    m_ui->fileView->setSelectionMode(QAbstractItemView::SingleSelection);

    setWindowTitle (tr ("QCloud"));
    setWindowIcon (QIcon::fromTheme ("qcloud"));

    connect (m_addAccountButton, SIGNAL (clicked (bool)), this, SLOT (addAccountButtonClicked()));
    connect (m_deleteAccountButton, SIGNAL(clicked(bool)), this, SLOT(deleteAccountButtonClicked()));
    connect (m_ui->fileView, SIGNAL(activated(QModelIndex)),this, SLOT(fileListActivated()));
    connect (m_listButton, SIGNAL(clicked(bool)),this, SLOT(listButtonClicked()));
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::addAccountButtonClicked()
{
    AccountDialog dialog;
    int result = dialog.exec();
    if (result == QDialog::Accepted && !dialog.accountType().isEmpty()) {
        ClientApp::instance()->client()->addAccount(dialog.accountType(), dialog.appName());
    }
}

void MainWindow::deleteAccountButtonClicked()
{
    if (!m_ui->accountView->currentIndex().isValid()){
        qDebug() << "Invalid index!";
        return ;
    }
    QModelIndex index;
    index = m_ui->accountView->currentIndex();
    QString uuid = static_cast<QCloud::Info*> (index.internalPointer())->name();
    if (uuid.isEmpty()){
        return ;
    }
    qDebug() << "Deteting account with ID : "<< uuid;
    qDebug() << "userName : " << static_cast<QCloud::Info*> (index.internalPointer())->displayName();
    ClientApp::instance()->client()->deleteAccount(uuid);
}

void MainWindow::accountsFinished(QDBusPendingCallWatcher* watcher)
{
    QDBusPendingReply< QCloud::InfoList > backends(*watcher);
    m_accountModel->setInfoList(backends.value());
}


void MainWindow::loadAccount()
{
    QDBusPendingReply< QCloud::InfoList > accounts = ClientApp::instance()->client()->listAccounts();
    QDBusPendingCallWatcher* appsWatcher = new QDBusPendingCallWatcher(accounts);
    connect(appsWatcher, SIGNAL(finished(QDBusPendingCallWatcher*)), this, SLOT(accountsFinished(QDBusPendingCallWatcher*)));
}

bool MainWindow::loadFileList()
{
    qDebug() << "Loading file list...";
    if (!m_ui->accountView->currentIndex().isValid()){
        qDebug() << "Invalid index!";
        return false;
    }
    QModelIndex index;
    index = m_ui->accountView->currentIndex();
    QString uuid = static_cast< QCloud::Info* > (index.internalPointer())->name();
    if (uuid.isEmpty()){
        return false;
    }
    QDBusPendingReply< int > id = ClientApp::instance()->client()->listFiles(uuid,currentDir);
    //ClientApp::instance()->client()->listFiles(uuid,currentDir);
    QEventLoop loop;
    QDBusPendingCallWatcher* appsWatcher = new QDBusPendingCallWatcher(id);
    connect(appsWatcher, SIGNAL(finished(QDBusPendingCallWatcher*)), &loop,SLOT(quit()));
    loop.exec();
    idSet.insert(id.value());
    idPath[id.value()] = currentDir;
    //connect(ClientApp::instance()->client(),SIGNAL(directoryInfoTransformed(QCloud::InfoList)),this,SLOT(fileListFinished(QCloud::InfoList)));
    connect(ClientApp::instance()->client(),SIGNAL(directoryInfoTransformed(int,uint,QCloud::EntryInfoList)),this,SLOT(fileListFinished(int,uint,QCloud::EntryInfoList)));
    return true;
}

void MainWindow::fileListActivated()
{    if (!m_ui->fileView->currentIndex().isValid()){
        qDebug() << "Invalid index!";
        return ;
    }
    QModelIndex index;
    index = m_ui->fileView->currentIndex();
    QString lastDir = currentDir;
    currentDir = static_cast<QCloud::EntryInfo*> (index.internalPointer())->path();
    bool is_dir = static_cast< QCloud::EntryInfo* > (index.internalPointer())->isDir();
    qDebug() << is_dir << " " << currentDir;
    if (!is_dir || (!loadFileList())){
        currentDir = lastDir;
        return ;
    }
}

void MainWindow::listButtonClicked()
{
    loadFileList();
}

void MainWindow::fileListFinished(int id, uint error, const QCloud::EntryInfoList& info)
{
    if (!idSet.contains(id))
        return ;
    idSet.remove(id);
    qDebug() << "Got file list info with ID : " << id;
    QFileInfo currentDirInfo(idPath[id]);
    QCloud::EntryInfo parentPath;
    parentPath.setValue(QCloud::EntryInfo::PathType,QVariant(currentDirInfo.path()));
    parentPath.setValue(QCloud::EntryInfo::DirType,QVariant(true));
    QCloud::EntryInfoList m_info = info;
    if (currentDirInfo.absolutePath()!="" && currentDirInfo.absolutePath()!=currentDirInfo.absoluteFilePath())
        m_info << parentPath;
    m_fileModel->setEntryInfoList(idPath[id],m_info);
}

