#include "serverconnector.h"
// #include "ui_serverconnector.h"

ServerConnector::ServerConnector(QWidget *parent)
    : QDialog(parent)
    // , ui(new Ui::ServerConnector)
    ,m_socket(nullptr)
{
    // ui->setupUi(this);
    // this->setWindowTitle("连接中心服务器");
}

ServerConnector& ServerConnector::instance()
{
    static ServerConnector instance;
    return instance;
}

bool ServerConnector::connectToServer(const QString& host, quint16 port)
{
    // 如果已经连接到同一个服务器，直接返回成功
    if (isConnected() && m_host == host && m_port == port) {
        DEBUG_LOCATION << "已经连接到服务器" << host << ":" << port;
        return true;
    }

    // 如果已连接到其他服务器，先断开
    if (isConnected()) {
        DEBUG_LOCATION << "断开当前连接，准备连接到新服务器";
        disconnect();
        // 注意：disconnect() 应该是同步的，或者等待它完成
    }

    m_host = host;
    m_port = port;

    m_socket = new QTcpSocket(this);

    connect(m_socket, &QTcpSocket::connected, this, &ServerConnector::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &ServerConnector::onDisconnected);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &ServerConnector::onError);
    connect(m_socket, &QTcpSocket::readyRead, this, &ServerConnector::onReadyRead);

    m_socket->connectToHost(host, port);
    return true;
}

void ServerConnector::disconnect()
{
    if (m_socket) {
        m_socket->disconnectFromHost();
    }
}

void ServerConnector::login(const QString& username, const QString& password)
{
    if (!isConnected()) {
        emit erring("未连接到服务器");
        return;
    }

    QJsonObject data;
    data["username"] = username;
    data["password"] = password;

    Message msg;
    msg.type = MessageType::LoginRequest;
    msg.data = data;

    sendMessage(msg);
    DEBUG_LOCATION << "发送登录请求:" << username
             << "msg.type: " << msg.type
                   << "msg.data: " << msg.data;
}

void ServerConnector::onConnected()
{
    DEBUG_LOCATION << "已连接到服务器" << m_host << ":" << m_port;
    emit connected();
}

void ServerConnector::onDisconnected()
{
    DEBUG_LOCATION << "与服务器断开连接";
    emit disconnected();
}

void ServerConnector::onError(QAbstractSocket::SocketError error)
{
    QString errorMsg = m_socket->errorString();
    qCritical() << "网络错误:" << errorMsg;
    emit erring(errorMsg);
}

void ServerConnector::onReadyRead()
{
    QByteArray data = m_socket->readAll();
    Message msg = Message::fromByteArray(data);

    switch (msg.type) {
    case MessageType::LoginResponse:
    {
        bool success = msg.data["success"].toBool();
        if (success) {
            emit loginResponse(true, msg.data);
        } else {
            QString error = msg.data["error"].toString();
            emit loginResponse(false, QJsonObject(), error);
        }
        break;
    }
    default:
        DEBUG_LOCATION << "收到未知消息类型:" << msg.type;
        break;
    }
}

void ServerConnector::sendMessage(const Message& msg)
{
    if (!isConnected()) return;
    m_socket->write(msg.toByteArray());
    m_socket->flush();
}
