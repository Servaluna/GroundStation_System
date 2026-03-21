#include "core/ui/mainwindow.h"
#include "core/ui/logindialog.h"
#include "core/database/databasemanager.h"
#include "core/network/serverconnector.h"
#include "core/network/deviceconnector.h"

#include <QApplication>
#include <QMessageBox>
#include <QTimer>
#include <QEventLoop>

#define DEBUG_LOCATION qDebug().nospace() \
<< "[" << Q_FUNC_INFO \
       << " at " << __FILE__ << ":" << __LINE__ << "]"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationName("GroundStation");

    DEBUG_LOCATION << "=== 地面站客户端启动 ===";

    // 1. 连接到服务器
    ServerConnector& client = ServerConnector::instance();
    client.connectToServer("localhost", 8000);

    // // 等待连接结果（简单等待1秒）
    // QThread::sleep(1);

    // 检查连接是否成功
    if (!client.isConnected()) {
        qCritical() << "无法连接到服务器 localhost:8000";
        qCritical() << "请确保服务器已启动";
        return -1;  // 直接结束程序
    }else {
        DEBUG_LOCATION << "服务器连接成功";
    }

    LoginDialog w;

    QObject::connect(&w, &LoginDialog::loginSuccess,
                     [&](const UserInfo& userInfo) {
                         MainWindow* mainWindow = new MainWindow(userInfo);
                         mainWindow->show();
                     });

    w.show();
    return a.exec();
}
