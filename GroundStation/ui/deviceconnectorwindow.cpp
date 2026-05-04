#include "deviceconnectorwindow.h"
#include "ui_deviceconnectorwindow.h"

deviceconnectorwindow::deviceconnectorwindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::deviceconnectorwindow)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowTitle("连接设备");
}

deviceconnectorwindow::~deviceconnectorwindow()
{
    delete ui;
}

void deviceconnectorwindow::on_btnConnect_clicked()
{
    QString IP = ui ->lineEdit_IP ->text();
    QString Port = ui ->lineEdit_Port ->text();

    m_socket ->connectToHost(QHostAddress(IP),Port.toUShort());

    connect(m_socket , &QTcpSocket::connected,[this](){
        QMessageBox::information(this,"连接提示","连接服务器成功");
    });
    connect(m_socket , &QTcpSocket::disconnected,[this](){
        QMessageBox::warning(this,"连接提示","网络异常连接失败");
    });
}


void deviceconnectorwindow::on_btnCancel_clicked()
{
    this -> close();
}

