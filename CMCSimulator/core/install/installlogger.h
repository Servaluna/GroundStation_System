#ifndef INSTALLLOGGER_H
#define INSTALLLOGGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QCoreApplication>

class InstallLogger : public QObject
{
    Q_OBJECT
public:
    static InstallLogger* instance();

    void logInstallStart(const QString& taskId, const QString& deviceId, const QString& fileName);
    void logInstallStep(const QString& taskId, const QString& step, int percent);
    void logInstallSuccess(const QString& taskId, const QString& newVersion, int elapsedMs);
    void logInstallFailure(const QString& taskId, const QString& error, bool rollbackSuccess);
    void logRollback(const QString& taskId, const QString& reason, bool success);

    void setLogFile(const QString& filePath);

    // explicit InstallLogger(QObject *parent = nullptr);

private:
    explicit InstallLogger(QObject* parent = nullptr);
    ~InstallLogger();

    void writeLog(const QString& level, const QString& message);

private:
    static InstallLogger* m_instance;
    QFile m_logFile;
    QTextStream m_logStream;
    QMutex m_mutex;

};

#endif // INSTALLLOGGER_H
