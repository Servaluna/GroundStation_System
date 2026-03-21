#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>
#include <QStatusBar>
#include <QCloseEvent>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

// 用户信息结构
struct UserInfo {
    int id;
    QString username;
    QString role;
    QString fullName;
    QString token;

    UserInfo() : id(-1) {}

    bool isValid() const { return id > 0; }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    explicit MainWindow(const UserInfo& userInfo,QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;    //重写关闭事件以进行退出确认

private slots:

    void on_btnExecute_clicked();

    void on_btnObtain_clicked();

    void on_btnLogs_clicked();

private:
    Ui::MainWindow *ui;

    UserInfo m_userInfo;

    void initUI();
};

#endif // MAINWINDOW_H
