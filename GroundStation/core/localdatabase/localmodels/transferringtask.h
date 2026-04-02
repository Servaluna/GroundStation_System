#ifndef TRANSFERRINGTASK_H
#define TRANSFERRINGTASK_H

#include "../Common/models.h"
#include <QString>
#include <QSqlRecord>

namespace TransferStatus  {
    enum Status {
        Pending = 0,        // 等待执行
        Downloading = 1,    // 下载中（从服务器下载文件）
        Uploading = 2,      // 上传中（发送给设备）
        Completed = 3,      // 已完成
        Failed = 4,         // 失败
        Cancelled = 5       // 已取消
    };

    inline QString toString(Status status) {
        switch(status) {
        case Pending: return "等待执行";
        case Downloading: return "下载中";
        case Uploading: return "上传中";
        case Completed: return "已完成";
        case Failed: return "失败";
        case Cancelled: return "已取消";
        default: return "未知";
        }
    }
}

class TransferringTask
{
public:
    TransferringTask();
    explicit TransferringTask(const TaskBasicInfo& taskInfo, const FileInfo& fileInfo);

    int id;                         // 自增主键
    QString taskId;                 // 服务器任务ID
    QString fileId;                 // 关联的文件ID

    // 任务信息
    TaskType::Type taskType;        // 任务类型
    QString description;            // 任务描述
    QString targetDeviceId;         // 目标设备ID
    int priority;                   // 优先级

    // 文件信息
    QString fileName;               // 文件名
    qint64 fileSize;                // 文件大小
    QString fileMd5;                // 文件MD5

    // 传输进度
    qint64 transferredBytes;        // 已传输字节数
    qint64 totalBytes;              // 总字节数

    // 状态信息
    TransferStatus ::Status status; // 本地状态
    QString currentStep;            // 当前步骤描述
    QString errorMessage;           // 错误信息

    // 时间信息
    QDateTime createTime;           // 创建时间
    QDateTime startTime;            // 开始时间
    QDateTime endTime;              // 结束时间
    QDateTime lastUpdateTime;       // 最后更新时间

    // 辅助方法
    bool isValid() const;
    int getProgressPercent() const;
    QString getStatusText() const;
    bool isRunning() const;
    bool isFinished() const;

    // 更新进度
    void updateProgress(qint64 bytesTransferred);
    void setStatus(TransferStatus ::Status newStatus);

    // JSON 序列化（用于传输）
    QJsonObject toJson() const;
    static TransferringTask fromJson(const QJsonObject& json);

    // 从数据库记录加载
    static TransferringTask fromSqlRecord(const QSqlRecord& record);
};

#endif // TRANSFERRINGTASK_H
