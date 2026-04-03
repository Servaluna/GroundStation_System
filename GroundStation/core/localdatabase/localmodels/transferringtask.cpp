#include "transferringtask.h"

TransferringTask::TransferringTask()
    : taskType(TaskType::Unknown)
    , priority(5)
    , fileSize(0)
    , transferredBytes(0)
    // , totalBytes(0)
    , status(TransferStatus ::Pending)
{}

TransferringTask::TransferringTask(const TaskBasicInfo& taskInfo, const FileInfo& fileInfo)
    : taskId(taskInfo.taskId)
    , fileId(fileInfo.fileId)
    , taskType(taskInfo.taskType)
    , description(taskInfo.description)

    , targetDeviceId(taskInfo.targetDeviceId)
    , priority(taskInfo.priority)

    , fileName(fileInfo.fileName)
    , fileSize(fileInfo.fileSize)
    , fileMd5(fileInfo.md5Hash)

    , transferredBytes(0)
    , status(TransferStatus ::Pending)
    , createTime(QDateTime::currentDateTime())

    , lastUpdateTime(QDateTime::currentDateTime())
{}

bool TransferringTask::isValid() const
{
    return !taskId.isEmpty() && !fileId.isEmpty() && fileSize > 0;
}

int TransferringTask::getProgressPercent() const
{
    if (fileSize <= 0) return 0;
    return static_cast<int>((transferredBytes * 100) / fileSize);
}

QString TransferringTask::getStatusText() const
{
    if (status == TransferStatus ::Downloading || status == TransferStatus ::Uploading) {
        return QString("%1 (%2%)").arg(TransferStatus ::toString(status)).arg(getProgressPercent());
    }
    return TransferStatus ::toString(status);
}

bool TransferringTask::isRunning() const
{
    return status == TransferStatus ::Downloading ||
           status == TransferStatus ::Uploading;
}

bool TransferringTask::isFinished() const
{
    return status == TransferStatus ::Succeeded  ||
           status == TransferStatus ::Failed ||
           status == TransferStatus ::Cancelled;
}

void TransferringTask::updateProgress(qint64 bytesTransferred)
{
    transferredBytes = bytesTransferred;
    lastUpdateTime = QDateTime::currentDateTime();
}

void TransferringTask::setStatus(TransferStatus ::Status newStatus)
{
    status = newStatus;
    lastUpdateTime = QDateTime::currentDateTime();

    if (newStatus == TransferStatus ::Downloading ||
        newStatus == TransferStatus ::Uploading) {
        if (startTime.isNull()) {
            startTime = QDateTime::currentDateTime();
        }
    }

    if (newStatus == TransferStatus ::Succeeded  ||
        newStatus == TransferStatus ::Failed) {
        endTime = QDateTime::currentDateTime();
    }
}

QJsonObject TransferringTask::toJson() const
{
    QJsonObject json;
    json["taskId"] = taskId;
    json["fileId"] = fileId;
    json["taskType"] = TaskType::toString(taskType);
    json["description"] = description;
    json["targetDeviceId"] = targetDeviceId;
    json["priority"] = priority;
    json["fileName"] = fileName;
    json["fileSize"] = qint64(fileSize);
    json["fileMd5"] = fileMd5;
    json["status"] = static_cast<int>(status);
    json["steps"] = steps;
    //

    return json;
}

TransferringTask TransferringTask::fromJson(const QJsonObject& json)
{
    TransferringTask task;
    task.taskId = json["taskId"].toString();
    task.fileId = json["fileId"].toString();
    task.taskType = TaskType::fromString(json["taskType"].toString());
    task.description = json["description"].toString();
    task.targetDeviceId = json["targetDeviceId"].toString();
    task.priority = json["priority"].toInt();
    task.fileName = json["fileName"].toString();
    task.fileSize = json["fileSize"].toInteger();
    task.fileMd5 = json["fileMd5"].toString();
    task.status = static_cast<TransferStatus ::Status>(json["status"].toInt());
    task.steps =  static_cast<CurrentSteps::Steps>(json["steps"].toInt());
    task.createTime = QDateTime::currentDateTime();
    task.lastUpdateTime = QDateTime::currentDateTime();
    //

    return task;
}
