#include "core/ui/logindialog.h"
#include "ui_logindialog.h"

#include "../network/serverconnector.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    init();
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::init()
{
    this->setWindowTitle("用户登录");
    ui->btnLogin->setDefault(true);

    ui->editUsername->setFocus();
    ui->editPassword->setEchoMode(QLineEdit::Password);
}


void LoginDialog::on_btnLogin_clicked()
{
    QString username = ui->editUsername->text().trimmed();
    QString password = ui->editPassword->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入用户名和密码");
        return;
    }

    // 发送登录请求
    ServerConnector::instance().loginRequest(username, password);
    DEBUG_LOCATION << "发送登录请求:" << username;

    ui->btnLogin->setEnabled(false);
    ui->btnLogin->setText("登录中...");

    // 等待响应（实际应用应该用异步处理）
}

void LoginDialog::on_btnCancel_clicked()
{
    reject();
}
