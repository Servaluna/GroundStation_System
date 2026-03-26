#ifndef DEVICESIMULATOR_H
#define DEVICESIMULATOR_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class DeviceSimulator;
}
QT_END_NAMESPACE

class DeviceSimulator : public QWidget
{
    Q_OBJECT

public:
    DeviceSimulator(QWidget *parent = nullptr);
    ~DeviceSimulator();

private:
    Ui::DeviceSimulator *ui;
};
#endif // DEVICESIMULATOR_H
