#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QObject>

class SessionManager : public QObject
{
    Q_OBJECT
public:
    explicit SessionManager(QObject *parent = nullptr);

signals:
};

#endif // SESSIONMANAGER_H
