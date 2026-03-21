#include "core/network/server.h"
#include "core/database/databasemanager.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DatabaseManager& dbManager = DatabaseManager::instance();

    QObject::connect(&dbManager, &DatabaseManager::connectionChanged,
                     [](bool connected) {
                         qDebug() << "Database connection changed:" << connected;
                     });

    if (!dbManager.initialize()) {
        qCritical() << "数据库初始化失败，程序退出:"
                    << dbManager.lastError();
        return -1;
    }

    Server server;
    QObject::connect(&server, &Server::logMessage,
                     [](const QString& msg) { qDebug() << "[Server]" << msg; });

    if (!server.start()) {
        qCritical() << "服务器启动失败";
        return -1;
    }

    qDebug() << "服务器运行中，按 Ctrl+C 退出";

    QSqlDatabase db = dbManager.getDatabase();//全局可用了
    QSqlQuery query(db);

    server.show();

    return a.exec();
}
