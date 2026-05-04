#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>

class AppController : public QObject
{
    Q_OBJECT
public:
    explicit AppController(QObject *parent = nullptr);

signals:
};

#endif // APPCONTROLLER_H
