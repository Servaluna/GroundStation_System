#include "deviceconnector.h"
#include "ui_deviceconnector.h"

DeviceConnector::DeviceConnector(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DeviceConnector)
{
    ui->setupUi(this);
    this->setWindowTitle("连接设备");

    m_socket = new QTcpSocket;
}

void DeviceConnector::on_btnCancel_clicked()
{
    this -> close();
}

void DeviceConnector::on_btnConnect_clicked()
{
    QString IP = ui ->lineEditIP ->text();
    QString Port = ui ->lineEditPort ->text();

    m_socket ->connectToHost(QHostAddress(IP),Port.toUShort());

    connect(m_socket , &QTcpSocket::connected,[this](){
        QMessageBox::information(this,"连接提示","连接服务器成功");
    });
    connect(m_socket , &QTcpSocket::disconnected,[this](){
        QMessageBox::warning(this,"连接提示","网络异常连接失败");
    });
}
