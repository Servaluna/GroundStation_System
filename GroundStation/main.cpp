#include "ui/mainwindow.h"
#include "ui/logindialog.h"

#include "core/network/serverconnector.h"
#include "core/network/deviceconnector.h"

#include <QApplication>
#include <QMessageBox>
#include <QTimer>
#include <QEventLoop>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("GroundStation");

    DEBUG_LOCATION << "=== 地面站客户端启动 ===";

    // 连接到Centralerver
    ServerConnector& client = ServerConnector::instance();  //一个GS只会有一个客户端只有一个serverconnector
    client.connectToServer("localhost", 8000);

    if (!client.isConnected()) {

        qCritical() << "无法连接到服务器Centralerver localhost:8000";
        qCritical() << "请确保服务器已启动";

        return -1;  // 直接结束程序
    }else {
        DEBUG_LOCATION << "=== 服务器Centralerver连接成功 ===";
    }

    LoginDialog* loginDialog = new LoginDialog();

    QObject::connect(&client, &ServerConnector::loginSuccess,
                     [&](QString token, const UserInfo& userInfo) {

        MainWindow* mainWindow = new MainWindow(token, userInfo);

        QObject::connect(mainWindow, &MainWindow::openDeviceConnector,
                         [&](){

            DeviceConnector* deviceConnector = new DeviceConnector();
            deviceConnector->show();
                         });

        QObject::connect(mainWindow, &MainWindow::logoutFromMainWindow,
                         [mainWindow](){

            mainWindow->close();
            mainWindow->deleteLater();

            LoginDialog* newloginDialog = new LoginDialog();
            newloginDialog->show();
                         });

        mainWindow->show();

        loginDialog->close();
        loginDialog->deleteLater();
    });

    loginDialog->show();
    return a.exec();
}
