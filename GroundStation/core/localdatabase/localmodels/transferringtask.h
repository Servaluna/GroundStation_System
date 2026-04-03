#ifndef TRANSFERRINGTASK_H
#define TRANSFERRINGTASK_H

#include "../Common/models.h"
#include <QString>
#include <QSqlRecord>

namespace TransferStatus  {
    enum Status {
        Pending,        // 等待执行
        Downloading,    // 下载中（从服务器下载文件）
        Uploading,      // 上传中（发送给设备）
        Installing,

        Paused,
        Succeeded,      // 已完成
        Failed,         // 失败
        Cancelled       // 已取消
    };

    inline QString toString(Status status) {
        switch(status) {
        case Pending: return "等待执行";
        case Downloading: return "下载中";
        case Uploading: return "上传中";
        case Succeeded : return "已完成";
        case Failed: return "失败";
        case Cancelled: return "已取消";
        default: return "未知";
        }
    }
}
namespace CurrentSteps {
    enum Steps {
        Idle,           // 空闲
        Preparing,

        Downloading,    // 下载中
        Verifying,

        Sending,        // 发送到设备
        Installing,     // 设备安装中
        Cleaning,

        Paused,
        Completed,      // 完成
        Failed,          // 失败
        Cancelled
    };
}


class TransferringTask
{
public:
    TransferringTask();
    explicit TransferringTask(const TaskBasicInfo& taskInfo, const FileInfo& fileInfo);

    // int id;                          // 自增主键
    QString taskId;                     // 服务器任务ID
    QString fileId;                     // 关联的文件ID

    // 任务信息
    TaskType::Type taskType;            // 任务类型
    QString description;                // 任务描述
    QString targetDeviceId;             // 目标设备ID
    int priority;                       // 优先级

    // 文件信息
    QString fileName;                   // 文件名
    qint64 fileSize;                    // 文件大小
    QString fileMd5;                    // 文件MD5

    // 传输进度
    qint64 transferredBytes;            // 已传输字节数

    // 状态信息
    TransferStatus ::Status status;     // 本地状态
    CurrentSteps::Steps steps;          // 当前步骤描述
    QString lastError;                  // 错误信息

    // 时间信息
    QDateTime createTime;               // 创建时间
    QDateTime startTime;                // 开始时间
    QDateTime endTime;                  // 结束时间
    QDateTime lastUpdateTime;           // 最后更新时间

    QString localCachePath;             //本地缓存路径
    QString localTempPath;              //临时文件路径

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
