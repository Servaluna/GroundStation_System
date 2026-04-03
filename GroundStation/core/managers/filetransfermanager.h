#ifndef FILETRANSFERMANAGER_H
#define FILETRANSFERMANAGER_H

#include <QObject>
#include <QFile>
#include <QTimer>
#include <QMap>

#include "../localdatabase/localmodels/transferringtask.h"
class ServerConnector;
class LocalDAO;

/**
 * @brief 文件传输管理器
 *
 * 职责：
 * 1. 从服务器下载文件（支持断点续传）
 * 2. 保存到本地缓存
 * 3. MD5验证
 * 4. 更新传输进度到数据库
 */
class FileTransferManager : public QObject
{
    Q_OBJECT
public:
    explicit FileTransferManager(QObject *parent = nullptr);
    ~FileTransferManager();

    /**
     * @brief 初始化管理器
     * @param serverConnector 服务器连接器（外部传入，不负责所有权）
     * @param dao 数据库访问对象（外部传入）
     * @return 是否初始化成功
     */
    bool init(ServerConnector* serverConnector, LocalDAO* dao);

    /**
     * @brief 开始下载文件
     * @param task 传输任务
     * @return 是否成功启动下载
     */
    bool startDownload(const TransferringTask& task);

    /**
     * @brief 暂停下载
     * @param taskId 任务ID
     * @return 是否成功暂停
     */
    bool pauseDownload(QString taskId);

    /**
     * @brief 恢复下载
     * @param taskId 任务ID
     * @return 是否成功恢复
     */
    bool resumeDownload(QString taskId);

    /**
     * @brief 取消下载
     * @param taskId 任务ID
     * @return 是否成功取消
     */
    bool cancelDownload(QString taskId);

    /**
     * @brief 获取下载进度
     * @param taskId 任务ID
     * @return 进度百分比（0-100）
     */
    int getProgress(QString taskId) const;

    /**
     * @brief 检查本地是否已有文件
     * @param taskId 任务ID
     * @param expectedMd5 期望的MD5
     * @return 文件是否有效
     */
    bool isLocalFileValid(QString taskId, const QString& expectedMd5) const;

signals:
    /**
     * @brief 下载进度更新
     * @param taskId 任务ID
     * @param transferred 已传输字节数
     * @param total 总字节数
     * @param progressPercent 进度百分比
     */
    void progressUpdated(QString taskId, qint64 transferred, qint64 total, int progressPercent);

    /**
     * @brief 下载完成
     * @param taskId 任务ID
     * @param localPath 本地文件路径
     * @param success 是否成功（MD5验证通过）
     */
    void downloadFinished(QString taskId, const QString& localPath, bool success);

    /**
     * @brief 下载失败
     * @param taskId 任务ID
     * @param errorCode 错误码
     * @param errorMessage 错误信息
     */
    void downloadFailed(QString taskId, int errorCode, const QString& errorMessage);

    /**
     * @brief 下载暂停
     * @param taskId 任务ID
     */
    void downloadPaused(QString taskId);

    /**
     * @brief 下载恢复
     * @param taskId 任务ID
     */
    void downloadResumed(QString taskId);

private slots:
    /**
     * @brief 处理服务器返回的文件块
     * @param taskId 任务ID
     * @param chunkData 文件块数据
     * @param chunkIndex 块索引
     * @param isLast 是否是最后一块
     */
    void onFileChunkReceived(QString taskId, const QByteArray& chunkData, int chunkIndex, bool isLast);

    /**
     * @brief 处理服务器返回的文件信息
     * @param taskId 任务ID
     * @param totalSize 文件总大小
     * @param md5 文件MD5
     */
    void onFileInfoReceived(QString taskId, qint64 totalSize, const QString& md5);

    /**
     * @brief 处理服务器错误
     * @param taskId 任务ID
     * @param errorCode 错误码
     * @param errorMessage 错误信息
     */
    void onServerError(QString taskId, int errorCode, const QString& errorMessage);

    /**
     * @brief 定时保存进度（防止数据丢失）
     */
    void onAutoSaveProgress();

private:
    /**
     * @brief 下载上下文（记录单个下载任务的状态）
     */
    struct DownloadContext {
        // TransferringTask task;

        QString taskId;
        QString localTempPath;      // 临时文件路径
        QString localCachePath;     // 最终文件路径
        QString fileName;
        qint64 totalSize;           // 文件总大小
        qint64 transferredBytes;    // 已传输字节数
        QString expectedMd5;        // 期望的MD5
        bool isPaused;              // 是否暂停
        bool isCancelled;           // 是否取消
        QFile* tempFile;            // 临时文件句柄
        int lastSavedProgress;      // 上次保存的进度（百分比）

        DownloadContext(const TransferringTask& t)
            : taskId(t.taskId)
            ,localTempPath(t.localTempPath)
            ,localCachePath(t.localCachePath)
            ,fileName(t.fileName)
            ,totalSize(t.fileSize)
            ,transferredBytes(t.transferredBytes)
            ,expectedMd5(t.fileMd5)

            , isPaused(false)
            , isCancelled(false)
            , tempFile(nullptr)
            , lastSavedProgress(-1) {}

        ~DownloadContext() {
            if (tempFile) {
                tempFile->close();
                delete tempFile;
                tempFile = nullptr;
            }
        }
    };
    /**
     * @brief 请求下载文件（发送请求到服务器）
     * @param taskId 任务ID
     * @param offset 起始偏移量（断点续传）
     * @return 是否成功发送请求
     */
    bool fileDownloadRequest(QString taskId, qint64 offset = 0);

    /**
     * @brief 计算文件MD5
     * @param filePath 文件路径
     * @return MD5字符串（小写）
     */
    QString calculateFileMd5(const QString& filePath) const;

    /**
     * @brief 保存当前进度到数据库
     * @param context 下载上下文
     */
    void saveProgressToDatabase(const DownloadContext& context);

    /**
     * @brief 完成下载（验证MD5并重命名文件）
     * @param context 下载上下文
     * @return 是否成功
     */
    bool finalizeDownload(DownloadContext& context);

    /**
     * @brief 清理下载上下文
     * @param taskId 任务ID
     * @param removeFile 是否删除临时文件
     */
    void cleanupContext(QString taskId, bool removeFile = true);

    /**
     * @brief 获取下载上下文
     * @param taskId 任务ID
     * @return 上下文指针（可能为nullptr）
     */
    DownloadContext* getContext(QString taskId);

private:
    ServerConnector* m_serverConnector;  // 服务器连接器（不负责所有权）
    LocalDAO* m_dao;                      // 数据库访问对象（不负责所有权）
    QMap<QString, DownloadContext*> m_downloads;  // 活动下载任务
    QTimer* m_autoSaveTimer;              // 自动保存定时器

    bool m_initialized;
};

#endif // FILETRANSFERMANAGER_H
