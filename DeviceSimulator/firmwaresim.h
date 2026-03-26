#ifndef FIRMWARESIM_H
#define FIRMWARESIM_H

#include <QObject>

class FirmwareSim : public QObject
{
    Q_OBJECT
public:
    explicit FirmwareSim(QObject *parent = nullptr);

signals:
};

#endif // FIRMWARESIM_H
