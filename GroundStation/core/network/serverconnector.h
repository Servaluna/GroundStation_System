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

class ServerConnector : public QDialog
{
    Q_OBJECT

public:
    static ServerConnector& instance();

    bool connectToServer(const QString& host, quint16 port);
    void disconnectFromServer();
    bool isConnected() const { return m_socket && m_socket->isOpen(); }

    void loginRequest(const QString& username, const QString& password);

signals:
    void connected();
    void disconnected();
    void erring(const QString& msg);

    void loginSuccess(QString token, const UserInfo& userInfo);

private slots:
    void onConnected();
    void onDisconnected();
    void onErrorOccurred(QAbstractSocket::SocketError error);
    void onReadyRead();

private:
    explicit ServerConnector(QWidget *parent = nullptr);
    ~ServerConnector();

    void cleanupSocket();
    void doConnect(const QString& host, quint16 port);

    void handleLoginResponse( const Message& respMsg);

    QTcpSocket* m_socket = nullptr;
    QString m_host;
    quint16 m_port;

    QString m_pendingHost;
    quint16 m_pendingPort;
    bool m_pendingConnection = false;

    QByteArray m_buffer;
    quint32 m_expectedLength = 0;
    quint64 m_lastActiveTime = 0;

};

#endif // SERVERCONNECTOR_H
