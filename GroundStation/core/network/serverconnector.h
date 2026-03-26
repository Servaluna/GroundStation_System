#ifndef SERVERCONNECTOR_H
#define SERVERCONNECTOR_H

#include "../Common/protocol.h"
#include "../Common/models.h"

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
    void loginRequest(const QString& username, const QString& password);

signals:
    void connected();
    void disconnected();
    void erring(const QString& msg);

    //登录响应信号
    // void loginResponse(bool success, const QJsonObject& data);
    void loginSuccess(QString token, const UserInfo& userInfo);

private slots:
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError error);

    void onReadyRead();

private:
    explicit ServerConnector(QWidget *parent = nullptr);

    void handleLoginResponse( const Message& respMsg);

    Ui::ServerConnector *ui;

    QTcpSocket* m_socket = nullptr;
    QString m_host;
    quint16 m_port;
    QByteArray m_buffer;
    quint32 m_expectedLength = 0;

    quint64 m_lastActiveTime = 0;

};

#endif // SERVERCONNECTOR_H
