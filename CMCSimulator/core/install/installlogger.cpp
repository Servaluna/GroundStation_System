#include "installlogger.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>

InstallLogger* InstallLogger::m_instance = nullptr;

InstallLogger* InstallLogger::instance()
{
    if (!m_instance) {
        m_instance = new InstallLogger();
    }
    return m_instance;
}

InstallLogger::InstallLogger(QObject *parent)
    : QObject{parent}
{
    // 默认日志文件路径
    QString logDir = QCoreApplication::applicationDirPath() + "/logs";
    QDir dir;
    if (!dir.exists(logDir)) {
        dir.mkpath(logDir);
    }

    QString logFilePath = logDir + "/install.log";
    m_logFile.setFileName(logFilePath);

    if (!m_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        qWarning() << "InstallLogger: Cannot open log file:" << logFilePath;
        return;
    }

    m_logStream.setDevice(&m_logFile);
}

InstallLogger::~InstallLogger()
{
    if (m_logFile.isOpen()) {
        m_logFile.close();
    }
}

void InstallLogger::setLogFile(const QString& filePath)
{
    QMutexLocker locker(&m_mutex);

    if (m_logFile.isOpen()) {
        m_logFile.close();
    }

    // 确保目录存在
    QFileInfo fileInfo(filePath);
    QDir dir;
    if (!dir.exists(fileInfo.absolutePath())) {
        dir.mkpath(fileInfo.absolutePath());
    }

    m_logFile.setFileName(filePath);
    if (m_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        m_logStream.setDevice(&m_logFile);
    } else {
        qWarning() << "InstallLogger: Cannot open log file:" << filePath;
    }
}

void InstallLogger::logInstallStart(const QString& taskId, const QString& deviceId, const QString& fileName)
{
    QString msg = QString::asprintf("START | taskId=%s | deviceId=%s | file=%s",
                                    taskId.toUtf8().constData(),
                                    deviceId.toUtf8().constData(),
                                    fileName.toUtf8().constData());
    writeLog("INFO", msg);
}

void InstallLogger::logInstallStep(const QString& taskId, const QString& step, int percent)
{
    QString msg = QString::asprintf("STEP | taskId=%s | step=%s | percent=%d%%",
                                    taskId.toUtf8().constData(),
                                    step.toUtf8().constData(),
                                    percent);
    writeLog("INFO", msg);
}

void InstallLogger::logInstallSuccess(const QString& taskId, const QString& newVersion, int elapsedMs)
{
    QString msg = QString::asprintf("SUCCESS | taskId=%s | newVersion=%s | elapsedMs=%d",
                                    taskId.toUtf8().constData(),
                                    newVersion.toUtf8().constData(),
                                    elapsedMs);
    writeLog("INFO", msg);
}

void InstallLogger::logInstallFailure(const QString& taskId, const QString& error, bool rollbackSuccess)
{
    QString msg = QString::asprintf("FAILURE | taskId=%s | error=%s | rollbackSuccess=%d",
                                    taskId.toUtf8().constData(),
                                    error.toUtf8().constData(),
                                    rollbackSuccess ? 1 : 0);
    writeLog("ERROR", msg);
}

void InstallLogger::logRollback(const QString& taskId, const QString& reason, bool success)
{
    QString msg = QString::asprintf("ROLLBACK | taskId=%s | reason=%s | success=%d",
                                    taskId.toUtf8().constData(),
                                    reason.toUtf8().constData(),
                                    success ? 1 : 0);
    writeLog("WARN", msg);
}

void InstallLogger::writeLog(const QString& level, const QString& message)
{
    QMutexLocker locker(&m_mutex);

    if (!m_logStream.device() || !m_logStream.device()->isOpen()) {
        return;
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    m_logStream << timestamp << " | " << level << " | " << message << Qt::endl;
    m_logStream.flush();
}
