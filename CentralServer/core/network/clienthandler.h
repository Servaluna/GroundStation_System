#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include "../Common/protocol.h"

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QFileInfo>

#define DEBUG_LOCATION qDebug().nospace()\
<< "[" << Q_FUNC_INFO\
       << " @ " << QFileInfo(__FILE__).fileName() << ":" << __LINE__ << "]"

class ClientHandler : public QObject
{
    Q_OBJECT
public:
    explicit ClientHandler(QTcpSocket* socket, QObject *parent = nullptr);
    ~ClientHandler();

signals:
    void finished();
    void logMessage(const QString& msg);

private slots:
    void onReadyRead();
    void onDisconnected();

private:
    QTcpSocket* m_socket;

    void handleLogin(const QJsonObject& data);
    void sendResponse(MessageType type, bool success, const QJsonObject& data = QJsonObject());

};

#endif // CLIENTHANDLER_H
