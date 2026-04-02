// #ifndef TASKEXECUTOR_H
// #define TASKEXECUTOR_H

// #include <QObject>
// #include <QTcpSocket>

// class TaskExecutor : public QObject
// {
//     Q_OBJECT
// public:
//     explicit TaskExecutor(QObject *parent = nullptr);

//     // 设置socket连接
//     void setServerSocket(QTcpSocket* socket);
//     void setDeviceSocket(QTcpSocket* socket);

//     // 执行任务
//     void executeTask(const QString& taskId);

//     // 获取当前进度
//     int progress() const { return m_progress; }
//     QString currentStep() const { return m_currentStep; }

// signals:
//     void progressUpdated(int percent, const QString& step);  // 进度和步骤
//     void taskCompleted(bool success, const QString& message);
//     void logMessage(const QString& msg);

// private slots:
//     void onServerMessage();
//     void onDeviceMessage();
//     void sendFileToDevice();

// private:
//     QTcpSocket* m_serverSocket;
//     QTcpSocket* m_deviceSocket;

//     // 状态
//     int m_progress;
//     QString m_currentStep;
//     QString m_currentTaskId;

//     // 文件相关
//     QString m_cacheFile;      // 缓存文件路径
//     QByteArray m_fileData;     // 文件数据（内存中）
//     int m_sentBytes;           // 已发送字节数

//     // 内部方法
//     bool saveToCache(const QByteArray& data, const QString& filename);
//     bool verifyFile(const QString& filePath, const QString& expectedMD5);
//     void cleanup();

// };

// #endif // TASKEXECUTOR_H
