#include "devicesimulator.h"
#include "ui_devicesimulator.h"

DeviceSimulator::DeviceSimulator(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DeviceSimulator)
{
    ui->setupUi(this);
}

DeviceSimulator::~DeviceSimulator()
{
    delete ui;
}
