#include "core/ui/logindialog.h"
#include "ui_logindialog.h"

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

    // 确认输入有东西
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入用户名和密码");
        return;
    }

    // 连接登录响应信号
    connect(&ServerConnector::instance(), &ServerConnector::loginResponse,
            this, &LoginDialog::handleLoginResponse, Qt::UniqueConnection);

    // 发送登录请求
    ui->btnLogin->setEnabled(false);
    ui->btnLogin->setText("登录中...");

    ServerConnector::instance().login(username, password);

    // 等待响应（实际应用应该用异步处理）
    // QEventLoop loop;
    // connect(&ServerConnector::instance(), &ServerConnector::loginResponse, &loop, &QEventLoop::quit);
    // QTimer::singleShot(5000, &loop, &QEventLoop::quit); // 5秒超时
    // loop.exec();

    // ui->btnLogin->setEnabled(true);
    // ui->btnLogin->setText("登录");
}

void LoginDialog::on_btnCancel_clicked()
{
    reject();
}

void LoginDialog::handleLoginResponse(bool success, const QJsonObject& data, const QString& error)
{
    if (success) {
        // 保存用户信息
        QJsonObject userObj = data["user"].toObject();
        m_userInfo.id = userObj["id"].toInt();
        m_userInfo.username = userObj["username"].toString();
        m_userInfo.role = userObj["role"].toString();
        m_userInfo.fullName = userObj["fullName"].toString();
        m_userInfo.token = data["token"].toString();  // 保存token供后续请求用

        qDebug() << "登录成功，用户:" << m_userInfo.username << "角色:" << m_userInfo.role;

        emit loginSuccess(m_userInfo);

        accept();  // 关闭登录对话框
    } else {
        QMessageBox::warning(this, "登录失败", error.isEmpty() ? "用户名或密码错误" : error);
        ui->editPassword->clear();
        ui->editPassword->setFocus();
    }
}
