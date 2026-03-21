#ifndef DEVICECONNECTOR_H
#define DEVICECONNECTOR_H

#include <QWidget>
#include <QTcpSocket>
#include <QMessageBox>

namespace Ui {
class DeviceConnector;
}

class DeviceConnector : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceConnector(QWidget *parent = nullptr);

private slots:
    void on_btnCancel_clicked();
    void on_btnConnect_clicked();

private:
    Ui::DeviceConnector *ui;
    QTcpSocket* m_socket;
};

#endif // DEVICECONNECTOR_H
