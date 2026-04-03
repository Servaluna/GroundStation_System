#include "filetransfermanager.h"
#include "../network/serverconnector.h"
#include "../localdatabase/localdao.h"

// #include "../Common/protocol.h"

#include <QDir>
#include <QCryptographicHash>

// 常量定义
constexpr int AUTO_SAVE_INTERVAL_MS = 5000;  // 每5秒自动保存一次进度
constexpr int PROGRESS_SAVE_THRESHOLD = 5;   // 进度变化超过5%才保存到数据库

FileTransferManager::FileTransferManager(QObject *parent)
    : QObject{parent}
    , m_serverConnector(nullptr)
    , m_dao(nullptr)
    , m_autoSaveTimer(nullptr)
    , m_initialized(false)
{}

FileTransferManager::~FileTransferManager()
{
    // 清理所有下载上下文
    for (auto context : m_downloads.values()) {
        if (context) {
            delete context;
        }
    }
    m_downloads.clear();

    if (m_autoSaveTimer) {
        m_autoSaveTimer->stop();
        delete m_autoSaveTimer;
    }
}

bool FileTransferManager::init(ServerConnector* serverConnector, LocalDAO* dao)
{
    if (!serverConnector || !dao) {
        qCritical() << "FileTransferManager::init - Invalid parameters";
        return false;
    }

    m_serverConnector = serverConnector;
    m_dao = dao;

    // 连接服务器信号
    connect(m_serverConnector, &ServerConnector::fileChunkReceived,
            this, &FileTransferManager::onFileChunkReceived);
    connect(m_serverConnector, &ServerConnector::fileInfoReceived,
            this, &FileTransferManager::onFileInfoReceived);
    connect(m_serverConnector, &ServerConnector::errorOccurred,
            this, &FileTransferManager::onServerError);

    // 创建自动保存定时器
    m_autoSaveTimer = new QTimer(this);
    connect(m_autoSaveTimer, &QTimer::timeout, this, &FileTransferManager::onAutoSaveProgress);
    m_autoSaveTimer->start(AUTO_SAVE_INTERVAL_MS);

    m_initialized = true;
    qDebug() << "FileTransferManager initialized";
    return true;
}

bool FileTransferManager::startDownload(const TransferringTask& task)
{
    if (!m_initialized) {
        qWarning() << "FileTransferManager not initialized";
        return false;
    }

    // 检查是否已在下载中
    if (m_downloads.contains(task.taskId)) {
        qWarning() << "Task" << task.taskId << "is already downloading";
        return false;
    }

    // 检查本地是否已有完整且有效的文件
    if (isLocalFileValid(task.taskId, task.fileMd5)) {
        qDebug() << "Task" << task.taskId << "file already exists and valid, skip download";
        emit downloadFinished(task.taskId, task.localCachePath, true);
        return true;
    }

    // 创建下载上下文
    DownloadContext* context = new DownloadContext(task);

    // 设置文件路径
    // QString cacheDir = "cache/files/";
    // QDir().mkpath(cacheDir);
    // context->localTempPath = cacheDir + QString("temp_%1_%2").arg(task.taskId).arg(task.fileName);
    // context->localCachePath = cacheDir + task.fileName;
    QDir().mkpath(QFileInfo(task.localCachePath).path());
    QDir().mkpath(QFileInfo(task.localTempPath).path());


    // 打开临时文件（追加模式，支持断点续传）
    context->tempFile = new QFile(context->localTempPath);
    if (context->transferredBytes > 0) {
        // 断点续传：打开现有文件
        if (!context->tempFile->open(QIODevice::ReadWrite)) {
            qCritical() << "Failed to open temp file for resume:" << context->localTempPath;
            delete context;
            return false;
        }

        // 验证现有文件大小是否匹配
        qint64 actualSize = context->tempFile->size();
        if (actualSize != context->transferredBytes) {
            qWarning() << "File size mismatch, resetting download";
            context->transferredBytes = 0;
            context->tempFile->resize(0);
        } else {
            context->tempFile->seek(context->transferredBytes);
        }
    } else {
        // 新下载：创建新文件
        if (!context->tempFile->open(QIODevice::WriteOnly)) {
            qCritical() << "Failed to create temp file:" << context->localTempPath;
            delete context;
            return false;
        }
    }

    m_downloads[task.taskId] = context;

    // 发送下载请求到服务器
    if (!fileDownloadRequest(task.taskId, context->transferredBytes)) {
        cleanupContext(task.taskId);
        return false;
    }

    qDebug() << "Started download for task" << task.taskId
             << "resume from" << context->transferredBytes << "bytes";
    return true;
}

bool FileTransferManager::pauseDownload(QString taskId)
{
    DownloadContext* context = getContext(taskId);
    if (!context) {
        qWarning() << "Task" << taskId << "not found";
        return false;
    }

    context->isPaused = true;
    saveProgressToDatabase(*context);
    emit downloadPaused(taskId);

    qDebug() << "Paused download for task" << taskId;
    return true;
}

bool FileTransferManager::resumeDownload(QString taskId)
{
    DownloadContext* context = getContext(taskId);
    if (!context) {
        qWarning() << "Task" << taskId << "not found";
        return false;
    }

    if (!context->isPaused) {
        qWarning() << "Task" << taskId << "is not paused";
        return false;
    }

    context->isPaused = false;

    // 重新请求下载
    if (!fileDownloadRequest(taskId, context->transferredBytes)) {
        return false;
    }

    emit downloadResumed(taskId);
    qDebug() << "Resumed download for task" << taskId;
    return true;
}

bool FileTransferManager::cancelDownload(QString taskId)
{
    DownloadContext* context = getContext(taskId);
    if (!context) {
        qWarning() << "Task" << taskId << "not found";
        return false;
    }

    context->isCancelled = true;
    cleanupContext(taskId, true);  // 删除临时文件
    qDebug() << "Cancelled download for task" << taskId;
    return true;
}

int FileTransferManager::getProgress(QString taskId) const
{
    const DownloadContext* context = m_downloads.value(taskId);
    if (context && context->totalSize > 0) {
        return static_cast<int>((context->transferredBytes * 100) / context->totalSize);
    }

    // 如果不在活动下载中，从数据库查询
    auto task = m_dao->getTransferringTaskById(taskId);
    if (!task.taskId.isEmpty() && task.fileSize > 0) {
        return static_cast<int>((task.transferredBytes * 100) / task.fileSize);
    }

    return 0;
}

bool FileTransferManager::isLocalFileValid(QString taskId, const QString& expectedMd5) const
{
    // 从数据库获取任务信息
    auto task = m_dao->getTransferringTaskById(taskId);
    if (task.taskId.isEmpty()) {
        return false;
    }

    // QString cacheDir = "cache/files/";
    QString localPath = task.localCachePath;

    QFileInfo fileInfo(localPath);
    if (!fileInfo.exists() || fileInfo.size() != task.fileSize) {
        return false;
    }

    // 计算MD5
    QString actualMd5 = const_cast<FileTransferManager*>(this)->calculateFileMd5(localPath);
    return actualMd5.compare(expectedMd5, Qt::CaseInsensitive) == 0;
}

void FileTransferManager::onFileChunkReceived(QString taskId, const QByteArray& chunkData, int chunkIndex, bool isLast)
{
    Q_UNUSED(chunkIndex);  // 告诉编译器这个参数是有意未使用的

    DownloadContext* context = getContext(taskId);
    if (!context || context->isPaused || context->isCancelled) {
        return;
    }

    // 写入数据
    qint64 bytesWritten = context->tempFile->write(chunkData);
    if (bytesWritten != chunkData.size()) {
        emit downloadFailed(taskId, 1001, "Failed to write to temp file");
        cleanupContext(taskId);
        return;
    }

    context->tempFile->flush();
    context->transferredBytes += bytesWritten;

    // 计算进度
    int progressPercent = 0;
    if (context->totalSize > 0) {
        progressPercent = static_cast<int>((context->transferredBytes * 100) / context->totalSize);
    }

    // 发送进度信号
    emit progressUpdated(taskId, context->transferredBytes, context->totalSize, progressPercent);

    // 自动保存进度（仅当进度变化超过阈值时）
    int progressChange = qAbs(progressPercent - context->lastSavedProgress);
    if (progressChange >= PROGRESS_SAVE_THRESHOLD) {
        saveProgressToDatabase(*context);
        context->lastSavedProgress = progressPercent;
    }

    // 检查是否完成
    if (isLast || context->transferredBytes >= context->totalSize) {
        qDebug() << "Download completed for task" << taskId;
        bool success = finalizeDownload(*context);
        if (success) {
            emit downloadFinished(taskId, context->localCachePath, true);
        } else {
            emit downloadFailed(taskId, 1002, "MD5 verification failed");
        }
        cleanupContext(taskId, !success);  // 失败时删除临时文件
    }
}

void FileTransferManager::onFileInfoReceived(QString taskId, qint64 totalSize, const QString& md5)
{
    DownloadContext* context = getContext(taskId);
    if (!context) {
        return;
    }

    // 验证文件信息
    if (totalSize != context->totalSize) {
        qWarning() << "File size mismatch:" << totalSize << "vs" << context->totalSize;
        emit downloadFailed(taskId, 1003, "File size mismatch");
        cleanupContext(taskId);
        return;
    }

    if (md5 != context->expectedMd5) {
        qWarning() << "MD5 mismatch:" << md5 << "vs" << context->expectedMd5;
        emit downloadFailed(taskId, 1004, "MD5 mismatch from server");
        cleanupContext(taskId);
        return;
    }

    qDebug() << "File info verified for task" << taskId;
}

void FileTransferManager::onServerError(QString taskId, int errorCode, const QString& errorMessage)
{
    DownloadContext* context = getContext(taskId);
    if (!context) {
        return;
    }

    emit downloadFailed(taskId, errorCode, errorMessage);
    cleanupContext(taskId);
}

void FileTransferManager::onAutoSaveProgress()
{
    // 保存所有活动下载的进度
    for (auto context : m_downloads.values()) {
        if (context && !context->isPaused && !context->isCancelled) {
            saveProgressToDatabase(*context);
        }
    }
}

bool FileTransferManager::fileDownloadRequest(QString taskId, qint64 offset)
{
    if (!m_serverConnector) {
        qCritical() << "ServerConnector is null";
        return false;
    }

    // 获取任务信息以获取file_id
    auto task = m_dao->getTransferringTaskById(taskId);
    if (task.taskId.isEmpty()) {
        qCritical() << "Task" << taskId << "not found in database";
        return false;
    }

    // 发送请求到服务器
    // 这里假设ServerConnector有requestFileChunk方法
    // 实际使用时需要根据你的ServerConnector API调整
    bool result = m_serverConnector->fileDownloadRequest(task.fileId, offset);
    if (!result) {
        qCritical() << "Failed to send download request for task" << taskId;
        return false;
    }

    return true;
}

QString FileTransferManager::calculateFileMd5(const QString& filePath) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return QString();
    }

    QCryptographicHash hash(QCryptographicHash::Md5);
    if (!hash.addData(&file)) {
        return QString();
    }

    return QString(hash.result().toHex());
}

void FileTransferManager::saveProgressToDatabase(const DownloadContext& context)
{
    if (!m_dao) {
        return;
    }

    auto task = m_dao->getTransferringTaskById(context.taskId);
    if (task.taskId.isEmpty()) {
        return;
    }

    task.transferredBytes = context.transferredBytes;
    task.status = TransferStatus::Downloading;
    task.lastError.clear();

    m_dao->update(task);
}

bool FileTransferManager::finalizeDownload(DownloadContext& context)
{
    // 关闭临时文件
    if (context.tempFile) {
        context.tempFile->close();
    }

    // 验证MD5
    QString actualMd5 = calculateFileMd5(context.localTempPath);
    if (actualMd5.compare(context.expectedMd5, Qt::CaseInsensitive) != 0) {
        qCritical() << "MD5 verification failed for task" << context.taskId
                    << "Expected:" << context.expectedMd5
                    << "Actual:" << actualMd5;
        return false;
    }

    // 重命名临时文件为最终文件
    QFile tempFile(context.localTempPath);
    if (tempFile.exists()) {
        // 如果最终文件已存在，先删除
        if (QFile::exists(context.localCachePath)) {
            QFile::remove(context.localCachePath);
        }

        if (!tempFile.rename(context.localCachePath)) {
            qCritical() << "Failed to rename temp file to" << context.localCachePath;
            return false;
        }
    }

    // 更新数据库：标记下载完成
    TransferringTask task = m_dao->getTransferringTaskById(context.taskId);
    if (!task.taskId.isEmpty()) {
        task.status = TransferStatus::Completed;
        task.transferredBytes = context.totalSize;
        task.localCachePath = context.localCachePath;
        m_dao->update(task);
    }

    qDebug() << "Download finalized for task" << context.taskId
             << "File saved to" << context.localCachePath;
    return true;
}

void FileTransferManager::cleanupContext(QString taskId, bool removeFile)
{
    DownloadContext* context = m_downloads.take(taskId);
    if (!context) {
        return;
    }

    if (removeFile && context->tempFile) {
        context->tempFile->close();
        QFile::remove(context->localTempPath);
    }

    delete context;
}

FileTransferManager::DownloadContext* FileTransferManager::getContext(QString taskId)
{
    return m_downloads.value(taskId, nullptr);
}
