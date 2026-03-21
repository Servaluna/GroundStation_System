#include "clienthandler.h"
#include "../database/models/user.h"

ClientHandler::ClientHandler(QTcpSocket* socket , QObject *parent)
    : QObject{parent}
    ,m_socket(socket)
{
    connect(m_socket, &QTcpSocket::readyRead, this, &ClientHandler::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &ClientHandler::onDisconnected);

    emit logMessage("新客户端连接: " + socket->peerAddress().toString());
}

ClientHandler::~ClientHandler()
{
    DEBUG_LOCATION << "ClientHandler 被删除:" << this;
}

void ClientHandler::onReadyRead()
{
    QByteArray data = m_socket->readAll();

    // 解析消息
    Message msg = Message::fromByteArray(data);

    switch (msg.type) {
    case MSG_LOGIN_REQUEST:
        handleLogin(msg.data);
        break;

    default:
        DEBUG_LOCATION << "未知消息类型:" << msg.type;
        break;
    }
}

void ClientHandler::onDisconnected()
{
    emit logMessage("客户端断开连接: " + m_socket->peerAddress().toString());
    emit finished();

    m_socket->deleteLater();
    this->deleteLater();
}

void ClientHandler::handleLogin(const QJsonObject& data)
{
    QString username = data["username"].toString();
    QString password = data["password"].toString();

    emit logMessage(QString("handleLogin登录请求: %1").arg(username));

    // 验证用户（调用User模型）
    UserInfo userInfo = User::authenticate(username, password);

    QJsonObject responseData;

    if (userInfo.isValid()) {
        // 登录成功
        emit logMessage(QString("登录成功: %1 (%2)").arg(username,userInfo.role));

        // 生成简单token（实际项目应该用JWT）
        QString token = QString("%1_%2_%3")
                            .arg(userInfo.id)
                            .arg(userInfo.username)
                            .arg(QDateTime::currentMSecsSinceEpoch());

        // 对token进行简单哈希（可选）
        QByteArray tokenHash = QCryptographicHash::hash(
                                   token.toUtf8(),
                                   QCryptographicHash::Sha256
                                   ).toHex();

        responseData["token"] = QString(tokenHash);
        responseData["user"] = QJsonObject{
            {"id", userInfo.id},
            {"username", userInfo.username},
            {"role", userInfo.role},
            {"fullName", userInfo.fullName}
        };

        sendResponse(MSG_LOGIN_RESPONSE, true, responseData);
        DEBUG_LOCATION<<responseData;
    } else {
        // 登录失败
        emit logMessage(QString("登录失败: %1").arg(username));
        responseData["error"] = "用户名或密码错误";
        sendResponse(MSG_LOGIN_RESPONSE, false, responseData);
    }
}

void ClientHandler::sendResponse(MessageType type, bool success, const QJsonObject& data)
{
    QJsonObject responseData = data;
    responseData["success"] = success;

    Message response;
    response.type = type;
    response.data = responseData;

    m_socket->write(response.toByteArray());
    m_socket->flush();
}

