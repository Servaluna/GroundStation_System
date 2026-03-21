#include "mainwindow.h"
#include "ui_mainwindow.h"

#define DEBUG_LOCATION qDebug().nospace()\
<< "[" << Q_FUNC_INFO\
       << " @ " << QFileInfo(__FILE__).fileName() << ":" << __LINE__ << "]"

MainWindow::MainWindow(const UserInfo& userInfo, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_userInfo(userInfo)
{
    ui->setupUi(this);
    initUI();
}

MainWindow::MainWindow( QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

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
    // 设置窗口标题（包含用户角色）
    QString title = QString("地面站系统 - [%1]").arg(m_userInfo.role);
    setWindowTitle(title);

    // resize(1024, 768);

    // 在状态栏显示欢迎信息
    QString welcome = QString("欢迎用户: %1 (%2)").arg(m_userInfo.username , m_userInfo.role);
    statusBar()->showMessage(welcome, 5000);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!m_userInfo.username.isEmpty()) {
        int ret = QMessageBox::question(this, "退出确认",
                                        QString("用户 %1 正在使用程序，是否退出账户并关闭程序？")
                                            .arg(m_userInfo.username),
                                        QMessageBox::Yes | QMessageBox::No);

        if (ret == QMessageBox::No) {
            event->ignore();
            return;
        }

        // 可选：保存用户数据
        // saveUserData(currentUsername);
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

