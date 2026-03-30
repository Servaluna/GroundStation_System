#include "mainwindow.h"
#include "ui_mainwindow.h"

#define DEBUG_LOCATION qDebug().nospace() << "[" << Q_FUNC_INFO << " @ " << QFileInfo(__FILE__).fileName() << ":" << __LINE__ << "]"


MainWindow::MainWindow( QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initUI();
}

MainWindow::MainWindow(QString token, const UserInfo& userInfo, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_token(token)
    , m_userInfo(userInfo)
{
    ui->setupUi(this);
    initUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initUI()
{
    DEBUG_LOCATION;

    QString title = QString("地面站系统 - [%1]").arg(m_userInfo.role);
    setWindowTitle(title);

    initButtonsByRole();

    QString welcome = QString("欢迎用户: %1 (%2)").arg(m_userInfo.username , m_userInfo.role);
    statusBar()->showMessage(welcome, 5000);
}
void MainWindow::initButtonsByRole()
{
    // 根据角色设置按钮可见性
    switch(UserRole::roleFromString(m_userInfo.role)) {
    case UserRole::Admin:
        // Admin可以看到所有按钮
        ui->btnExecute->setVisible(true);
        ui->btnObtain->setVisible(true);
        ui->btnLogs->setVisible(true);
        break;

    case UserRole::Engineer:
        // Engineer只能看到Execute和Obtain
        ui->btnExecute->setVisible(true);
        ui->btnObtain->setVisible(true);
        ui->btnLogs->setVisible(false);
        break;

    case UserRole::Operator:
        // Operator只能看到Execute
        ui->btnExecute->setVisible(true);
        ui->btnObtain->setVisible(false);
        ui->btnLogs->setVisible(false);
        break;

    default:
        break;
    }
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!m_userInfo.username.isEmpty()) {
        int ret = QMessageBox::question(this, "退出确认",
                                        QString("用户 %1 正在使用程序，是否退出账户并关闭程序？").arg(m_userInfo.username),
                                        QMessageBox::Yes | QMessageBox::No);
        if (ret == QMessageBox::No) {
            event->ignore();
            return;
        }
    }

    event->accept();    // 接受关闭事件
    DEBUG_LOCATION << QString("用户 %1 退出程序").arg(m_userInfo.username);
}

void MainWindow::on_btnExecute_clicked()
{
   ui->stackedWidget ->setCurrentIndex(0);
}

void MainWindow::on_btnObtain_clicked()
{
    ui->stackedWidget ->setCurrentIndex(1);
}

void MainWindow::on_btnLogs_clicked()
{
    ui->stackedWidget ->setCurrentIndex(2);
}


void MainWindow::on_btnConnectToDevices_clicked()
{
    emit openDeviceConnector();
}


void MainWindow::on_btnLogout_clicked()
{
    emit logoutFromMainWindow();
}

