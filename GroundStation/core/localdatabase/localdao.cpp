#include "localdao.h"
#include "localdatabase.h"

#include <QSqlError>

LocalDAO::LocalDAO() {}

bool LocalDAO::insert(const TransferringTask& task)
{
    if (!task.isValid()) {
        qWarning() << "任务无效，无法插入";
        return false;
    }

    QString sql = R"(
        INSERT INTO transferring_tasks (
            task_id, file_id, task_type, description, target_device_id,
            priority, file_name, file_size, file_md5, transferred_bytes,
            status, current_step, error_message,
            create_time, start_time, end_time, last_update_time
        ) VALUES (
            :task_id, :file_id, :task_type, :description, :target_device_id,
            :priority, :file_name, :file_size, :file_md5, :transferred_bytes,
            :status, :current_step, :error_message,
            :create_time, :start_time, :end_time, :last_update_time
        )
    )";

    QSqlQuery query(LocalDatabase::getInstance()->getDatabase());
    query.prepare(sql);

    query.bindValue(":task_id", task.taskId);
    query.bindValue(":file_id", task.fileId);
    query.bindValue(":task_type", static_cast<int>(task.taskType));
    query.bindValue(":description", task.description);
    query.bindValue(":target_device_id", task.targetDeviceId);
    query.bindValue(":priority", task.priority);
    query.bindValue(":file_name", task.fileName);
    query.bindValue(":file_size", qint64(task.fileSize));
    query.bindValue(":file_md5", task.fileMd5);
    query.bindValue(":transferred_bytes", qint64(task.transferredBytes));
    query.bindValue(":status", static_cast<int>(task.status));
    query.bindValue(":current_step", task.steps);
    query.bindValue(":error_message", task.lastError);
    query.bindValue(":create_time", task.createTime.toSecsSinceEpoch());
    query.bindValue(":start_time", task.startTime.toSecsSinceEpoch());
    query.bindValue(":end_time", task.endTime.toSecsSinceEpoch());
    query.bindValue(":last_update_time", task.lastUpdateTime.toSecsSinceEpoch());

    if (!query.exec()) {
        qWarning() << "插入任务失败:" << query.lastError().text();
        return false;
    }

    return true;
}

bool LocalDAO::update(const TransferringTask& task)
{
    QString sql = R"(
        UPDATE transferring_tasks SET
            task_type = :task_type,
            description = :description,
            target_device_id = :target_device_id,
            priority = :priority,
            file_name = :file_name,
            file_size = :file_size,
            file_md5 = :file_md5,
            transferred_bytes = :transferred_bytes,
            status = :status,
            current_step = :current_step,
            last_error = :last_error,
            start_time = :start_time,
            end_time = :end_time,
            last_update_time = :last_update_time
        WHERE task_id = :task_id
    )";

    QSqlQuery query(LocalDatabase::getInstance()->getDatabase());
    query.prepare(sql);

    query.bindValue(":task_id", task.taskId);
    query.bindValue(":task_type", static_cast<int>(task.taskType));
    query.bindValue(":description", task.description);
    query.bindValue(":target_device_id", task.targetDeviceId);
    query.bindValue(":priority", task.priority);
    query.bindValue(":file_name", task.fileName);
    query.bindValue(":file_size", qint64(task.fileSize));
    query.bindValue(":file_md5", task.fileMd5);
    query.bindValue(":transferred_bytes", qint64(task.transferredBytes));
    query.bindValue(":status", static_cast<int>(task.status));
    query.bindValue(":current_step", task.steps);
    query.bindValue(":last_error", task.lastError);
    query.bindValue(":start_time", task.startTime.toSecsSinceEpoch());
    query.bindValue(":end_time", task.endTime.toSecsSinceEpoch());
    query.bindValue(":last_update_time", task.lastUpdateTime.toSecsSinceEpoch());

    if (!query.exec()) {
        qWarning() << "更新任务失败:" << query.lastError().text();
        return false;
    }

    return true;
}

bool LocalDAO::remove(int id)
{
    QString sql = "DELETE FROM transferring_tasks WHERE id = :id";
    QSqlQuery query(LocalDatabase::getInstance()->getDatabase());
    query.prepare(sql);
    query.bindValue(":id", id);

    return query.exec();
}

bool LocalDAO::removeByTaskId(const QString& taskId)
{
    QString sql = "DELETE FROM transferring_tasks WHERE task_id = :task_id";
    QSqlQuery query(LocalDatabase::getInstance()->getDatabase());
    query.prepare(sql);
    query.bindValue(":task_id", taskId);

    return query.exec();
}

void LocalDAO::clearCompleted()
{
    QString sql = "DELETE FROM transferring_tasks WHERE status IN (4, 5, 6)";
    LocalDatabase::getInstance()->executeQuery(sql);
}

void LocalDAO::clearAll()
{
    LocalDatabase::getInstance()->executeQuery("DELETE FROM transferring_tasks");
}

TransferringTask LocalDAO::getTransferringTaskById(int id)
{
    QString sql = QString("SELECT * FROM transferring_tasks WHERE id = %1").arg(id);
    QSqlQuery query = LocalDatabase::getInstance()->executeQueryWithResult(sql);

    if (query.next()) {
        return rowToTask(query);
    }

    return TransferringTask();
}

TransferringTask LocalDAO::getTransferringTaskById(const QString& taskId)
{
    QString sql = "SELECT * FROM transferring_tasks WHERE task_id = :task_id";
    QSqlQuery query(LocalDatabase::getInstance()->getDatabase());
    query.prepare(sql);
    query.bindValue(":task_id", taskId);

    if (query.exec() && query.next()) {
        return rowToTask(query);
    }

    return TransferringTask();
}

QList<TransferringTask> LocalDAO::getAll()
{
    QList<TransferringTask> tasks;
    QString sql = "SELECT * FROM transferring_tasks ORDER BY priority DESC, create_time ASC";
    QSqlQuery query = LocalDatabase::getInstance()->executeQueryWithResult(sql);

    while (query.next()) {
        tasks.append(rowToTask(query));
    }

    return tasks;
}

QList<TransferringTask> LocalDAO::getByStatus(TransferStatus::Status status)
{
    QList<TransferringTask> tasks;
    QString sql = "SELECT * FROM transferring_tasks WHERE status = :status ORDER BY priority DESC, create_time ASC";
    QSqlQuery query(LocalDatabase::getInstance()->getDatabase());
    query.prepare(sql);
    query.bindValue(":status", static_cast<int>(status));

    if (query.exec()) {
        while (query.next()) {
            tasks.append(rowToTask(query));
        }
    }

    return tasks;
}

QList<TransferringTask> LocalDAO::getPendingTasks()
{
    // 获取状态为 Pending(0) 的任务
    return getByStatus(TransferStatus::Pending);
}

QList<TransferringTask> LocalDAO::getRunningTasks()
{
    QList<TransferringTask> tasks;
    QString sql = "SELECT * FROM transferring_tasks WHERE status IN (1, 2, 3) ORDER BY priority DESC, create_time ASC";
    QSqlQuery query = LocalDatabase::getInstance()->executeQueryWithResult(sql);

    while (query.next()) {
        tasks.append(rowToTask(query));
    }

    return tasks;
}

TransferringTask LocalDAO::getCurrentRunningTask()
{
    // 获取优先级最高、最早创建的运行中任务
    QString sql = "SELECT * FROM transferring_tasks WHERE status IN (1, 2, 3) ORDER BY priority DESC, create_time ASC LIMIT 1";
    QSqlQuery query = LocalDatabase::getInstance()->executeQueryWithResult(sql);

    if (query.next()) {
        return rowToTask(query);
    }

    return TransferringTask();
}

int LocalDAO::getPendingCount()
{
    QString sql = "SELECT COUNT(*) FROM transferring_tasks WHERE status = 0";
    QSqlQuery query = LocalDatabase::getInstance()->executeQueryWithResult(sql);

    if (query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

int LocalDAO::getRunningCount()
{
    QString sql = "SELECT COUNT(*) FROM transferring_tasks WHERE status IN (1, 2, 3)";
    QSqlQuery query = LocalDatabase::getInstance()->executeQueryWithResult(sql);

    if (query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

int LocalDAO::getCompletedCount()
{
    QString sql = "SELECT COUNT(*) FROM transferring_tasks WHERE status = 4";
    QSqlQuery query = LocalDatabase::getInstance()->executeQueryWithResult(sql);

    if (query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

int LocalDAO::getFailedCount()
{
    QString sql = "SELECT COUNT(*) FROM transferring_tasks WHERE status = 5";
    QSqlQuery query = LocalDatabase::getInstance()->executeQueryWithResult(sql);

    if (query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

bool LocalDAO::updateStatus(const QString& taskId, TransferStatus::Status status)
{
    QString sql = "UPDATE transferring_tasks SET status = :status, last_update_time = :last_update_time WHERE task_id = :task_id";
    QSqlQuery query(LocalDatabase::getInstance()->getDatabase());
    query.prepare(sql);
    query.bindValue(":status", static_cast<int>(status));
    query.bindValue(":last_update_time", QDateTime::currentSecsSinceEpoch());
    query.bindValue(":task_id", taskId);

    return query.exec();
}

bool LocalDAO::updateProgress(const QString& taskId, qint64 transferredBytes)
{
    QString sql = "UPDATE transferring_tasks SET transferred_bytes = :transferred_bytes, last_update_time = :last_update_time WHERE task_id = :task_id";
    QSqlQuery query(LocalDatabase::getInstance()->getDatabase());
    query.prepare(sql);
    query.bindValue(":transferred_bytes", qint64(transferredBytes));
    query.bindValue(":last_update_time", QDateTime::currentSecsSinceEpoch());
    query.bindValue(":task_id", taskId);

    return query.exec();
}

bool LocalDAO::updateCurrentStep(const QString& taskId, const QString& step)
{
    QString sql = "UPDATE transferring_tasks SET current_step = :current_step, last_update_time = :last_update_time WHERE task_id = :task_id";
    QSqlQuery query(LocalDatabase::getInstance()->getDatabase());
    query.prepare(sql);
    query.bindValue(":current_step", step);
    query.bindValue(":last_update_time", QDateTime::currentSecsSinceEpoch());
    query.bindValue(":task_id", taskId);

    return query.exec();
}

TransferringTask LocalDAO::rowToTask(const QSqlQuery& query)
{
    TransferringTask task;
    // task.id = query.value("id").toInt();
    task.taskId = query.value("task_id").toString();
    task.fileId = query.value("file_id").toString();
    task.taskType = static_cast<TaskType::Type>(query.value("task_type").toInt());
    task.description = query.value("description").toString();
    task.targetDeviceId = query.value("target_device_id").toString();
    task.priority = query.value("priority").toInt();
    task.fileName = query.value("file_name").toString();
    task.fileSize = query.value("file_size").toLongLong();
    task.fileMd5 = query.value("file_md5").toString();
    task.transferredBytes = query.value("transferred_bytes").toLongLong();
    task.status = static_cast<TransferStatus::Status>(query.value("status").toInt());
    task.steps = query.value("current_step").toString();
    task.lastError = query.value("error_message").toString();

    qint64 createTime = query.value("create_time").toLongLong();
    qint64 startTime = query.value("start_time").toLongLong();
    qint64 endTime = query.value("end_time").toLongLong();
    qint64 lastUpdateTime = query.value("last_update_time").toLongLong();

    task.createTime = QDateTime::fromSecsSinceEpoch(createTime);
    task.startTime = startTime > 0 ? QDateTime::fromSecsSinceEpoch(startTime) : QDateTime();
    task.endTime = endTime > 0 ? QDateTime::fromSecsSinceEpoch(endTime) : QDateTime();
    task.lastUpdateTime = QDateTime::fromSecsSinceEpoch(lastUpdateTime);

    return task;
}
