#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>
#include <QStatusBar>
#include <QCloseEvent>
#include <QMessageBox>

#include "../Common/models.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    explicit MainWindow(QString token, const UserInfo& userInfo,QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;    //重写关闭事件以进行退出确认

private slots:

    void on_btnExecute_clicked();

    void on_btnObtain_clicked();

    void on_btnLogs_clicked();

private:
    void initUI();

    Ui::MainWindow *ui;
    UserInfo m_userInfo;
    QString m_token;


};

#endif // MAINWINDOW_H
