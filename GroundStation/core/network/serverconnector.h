#ifndef SERVERCONNECTOR_H
#define SERVERCONNECTOR_H

#include "../Common/protocol.h"

#include <QDialog>
#include <QMessageBox>
#include <QTcpSocket>
#include <QHostAddress>
#include <QJsonObject>
#include <QFileInfo>

#define DEBUG_LOCATION qDebug().nospace()\
<< "[" << Q_FUNC_INFO\
       << " @ " << QFileInfo(__FILE__).fileName() << ":" << __LINE__ << "]"

namespace Ui {
class ServerConnector;
}

class ServerConnector : public QDialog
{
    Q_OBJECT

public:
    static ServerConnector& instance();

    bool connectToServer(const QString& host, quint16 port);
    void disconnect();
    bool isConnected() const { return m_socket && m_socket->isOpen(); }

    // 登录请求
    void login(const QString& username, const QString& password);

signals:
    void connected();
    void disconnected();
    void erring(const QString& msg);
    void loginResponse(bool success, const QJsonObject& data, const QString& error = "");

private slots:
    // void on_btnCancel_clicked();
    // void on_btnConnect_clicked();

    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError error);
    void onReadyRead();

private:
    explicit ServerConnector(QWidget *parent = nullptr);

    Ui::ServerConnector *ui;
    QTcpSocket* m_socket;
    QString m_host;
    quint16 m_port;

    void sendMessage(const Message& msg);
};

#endif // SERVERCONNECTOR_H
