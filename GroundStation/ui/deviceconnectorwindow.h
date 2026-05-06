#ifndef DEVICECONNECTORWINDOW_H
#define DEVICECONNECTORWINDOW_H

#include <QWidget>
#include <QTcpSocket>
#include <QMessageBox>

namespace Ui {
class deviceconnectorwindow;
}

class deviceconnectorwindow : public QWidget
{
    Q_OBJECT

public:
    explicit deviceconnectorwindow(QWidget *parent = nullptr);
    ~deviceconnectorwindow();

private slots:
    void on_btnConnect_clicked();
    void on_btnCancel_clicked();

private:
    Ui::deviceconnectorwindow *ui;
    QTcpSocket* m_socket = nullptr;
};

#endif // DEVICECONNECTORWINDOW_H
