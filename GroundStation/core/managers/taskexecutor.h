#ifndef TASKEXECUTOR_H
#define TASKEXECUTOR_H

#include <QObject>
#include <QTcpSocket>
#include <QElapsedTimer>
#include <QTimer>
#include <QQueue>

class LocalDAO;
class FileTransferManager;
class DeviceConnector;
// struct TransferringTask;
#include "../localdatabase/localmodels/transferringtask.h"

/**
 * @brief 任务执行器（操作员侧）
 *
 * 职责：
 * 1. 执行操作员触发的升级任务
 * 2. 串行执行（下载 → 发送 → 安装）
 * 3. 实时更新本地数据库状态
 * 4. 支持断点续传
 * 5. 支持暂停/恢复/取消
 *
 * 注意：本类不负责与中心服务器通信，只操作本地数据库
 */
class TaskExecutor : public QObject
{
    Q_OBJECT
public:
    explicit TaskExecutor(QObject *parent = nullptr);
    ~TaskExecutor();

    /**
     * @brief 初始化执行器
     * @param dao 本地数据库访问对象
     * @param fileTransferManager 文件传输管理器
     * @param deviceConnector 设备连接器
     * @return 是否初始化成功
     */
    bool init(LocalDAO* dao,
              FileTransferManager* fileTransferManager,
              DeviceConnector* deviceConnector);

    /**
     * @brief 启动执行器（开始处理队列）
     */
    void start();

    /**
     * @brief 停止执行器
     */
    void stop();

    /**
     * @brief 执行任务（操作员点击"执行"时调用）
     * @param taskId 任务ID
     * @return 是否成功加入队列
     */
    bool executeTask(const QString& taskId);

    /**
     * @brief 取消任务
     * @param taskId 任务ID
     * @return 是否成功取消
     */
    bool cancelTask(const QString& taskId);

    /**
     * @brief 暂停当前任务
     */
    void pauseCurrentTask();

    /**
     * @brief 恢复当前任务
     */
    void resumeCurrentTask();

    /**
     * @brief 获取当前执行的任务
     */
    TransferringTask getCurrentTask() const;

    /**
     * @brief 获取队列中待执行的任务数量
     */
    int getPendingCount() const;

    /**
     * @brief 是否有任务正在执行
     */
    bool isBusy() const;

signals:
    /**
     * @brief 任务开始执行
     * @param taskId 任务ID
     * @param taskName 任务名称
     */
    void taskStarted(QString taskId, QString taskName);

    /**
     * @brief 任务进度更新
     * @param taskId 任务ID
     * @param step 当前步骤（下载中/发送中/安装中）
     * @param progress 进度百分比
     * @param speed 当前速度（字节/秒，仅下载阶段有效）
     */
    void taskProgressUpdated(QString taskId, QString step, int progress, qint64 speed = 0);

    /**
     * @brief 任务完成
     * @param taskId 任务ID
     * @param success 是否成功
     * @param message 结果消息
     */
    void taskFinished(QString taskId, bool success, QString message);

    /**
     * @brief 队列状态变化
     * @param pendingCount 待执行任务数
     * @param isRunning 是否有任务运行中
     */
    void queueStatusChanged(int pendingCount, bool isRunning);

private slots:
    /**
     * @brief 处理下载进度
     */
    void onDownloadProgress(QString taskId, qint64 transferred, qint64 total, int progressPercent);

    /**
     * @brief 处理下载完成
     */
    void onDownloadFinished(QString taskId, const QString& localPath, bool success);

    /**
     * @brief 处理下载失败
     */
    void onDownloadFailed(QString taskId, int errorCode, const QString& errorMessage);

    /**
     * @brief 处理设备发送完成
     */
    void onDeviceSendFinished(QString taskId, bool success, const QString& message);

    /**
     * @brief 处理设备安装结果
     */
    void onDeviceInstallResult(QString taskId, bool success, const QString& message);

    /**
     * @brief 定时处理下一个任务
     */
    void onProcessNextTask();

    /**
     * @brief 定时更新下载速度
     */
    void onUpdateDownloadSpeed();

private:
    // /**
    //  * @brief 任务执行步骤
    //  */
    // enum class TaskStep {
    //     Idle,           // 空闲
    //     Downloading,    // 下载中
    //     Sending,        // 发送到设备
    //     Installing,     // 设备安装中
    //     Completed,      // 完成
    //     Failed          // 失败
    // };

    /**
     * @brief 处理下一个任务
     */
    void processNextTask();

    /**
     * @brief 开始下载任务
     */
    void startDownloadTask(const TransferringTask& task);

    /**
     * @brief 开始发送文件到设备
     */
    void startSendToDevice(const QString& taskId, const QString& localPath);

    /**
     * @brief 更新本地数据库任务状态
     */
    void updateLocalTaskStatus(const QString& taskId,
                               TransferStatus::Status status,
                               CurrentSteps::Steps steps,
                               const QString& errorMessage = QString());

    /**
     * @brief 更新本地数据库任务进度
     */
    void updateLocalTaskProgress(const QString& taskId, qint64 transferredBytes, int progress);

    /**
     * @brief 标记任务完成（待同步）
     */
    void markTaskComplete(const QString& taskId, bool success, const QString& message);

    /**
     * @brief 加载可恢复的任务（程序启动时）
     */
    void loadResumableTasks();

    /**
     * @brief 检查设备是否在线
     */
    bool isDeviceOnline(const QString& deviceId);

    /**
     * @brief 计算下载速度
     */
    void calculateSpeed(qint64 bytesTransferred);

private:
    // 依赖组件
    LocalDAO* m_dao;
    FileTransferManager* m_fileManager;
    DeviceConnector* m_deviceConnector;

    // 队列管理
    QQueue<TransferringTask> m_taskQueue;
    TransferringTask m_currentTask;
    CurrentSteps::Steps m_currentStep;

    // 执行控制
    QTimer* m_processTimer;
    bool m_isRunning;
    bool m_initialized;

    // 重试机制
    int m_retryCount;
    static constexpr int MAX_RETRY_COUNT = 3;
    static constexpr int RETRY_DELAY_MS = 2000;

    // 速度统计
    QElapsedTimer m_speedTimer;
    qint64 m_lastTransferredBytes;
    qint64 m_currentSpeed;
    QTimer* m_speedUpdateTimer;

};

#endif // TASKEXECUTOR_H
