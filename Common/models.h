#ifndef MODELS_H
#define MODELS_H

#include "qjsonobject.h"

#include <QString>
#include <QDateTime>


//users表

namespace UserRole {
    enum Role{
        Admin = 0,
        Engineer = 1,
        Operator = 2
    };

    inline Role roleFromString(const QString& roleStr) {
        if (roleStr == "Admin") return Admin;
        if (roleStr == "Engineer") return Engineer;
        if (roleStr == "Operator") return Operator;
        return Operator; // 默认值
    }

    inline QString roleToString(Role role) {
        switch(role) {
        case Admin: return "Admin";
        case Engineer: return "Engineer";
        case Operator: return "Operator";
        default: return "Operator";
        }
    }
}

struct UserInfo {
    int id;
    QString username;
    QString passwordHash;
    QString fullName;
    QString role;           // 'Operator', 'Engineer', 'Admin'

    bool isActive;
    QDateTime lastLogin;

    UserInfo() : id(-1), isActive(true) {}

    bool isValid() const { return id > 0; }

    bool isAdmin() const { return role == "Admin"; }
    bool isEngineer() const { return role == "Engineer"; }
    bool isOperator() const { return role == "Operator"; }

    static UserInfo fromJson(const QJsonObject& json) {
        UserInfo info;
        info.id = json["id"].toInt();
        info.username = json["username"].toString();
        // info.passwordHash = json["passwordHash"].toString();
        info.fullName = json["fullName"].toString();
        info.role = json["role"].toString();
        return info;
    }

    QJsonObject toJson() const {
        QJsonObject json;
        json["id"] = id;
        json["username"] = username;
        json["passwordHash"] = passwordHash;
        json["fullName"] = fullName;
        json["role"] = role;
        return json;
    }

};


//aircrafts表

struct AircraftInfo {
    QString aircraftId;
    QString model;
    QString manufacturer;
    QString serialNumber;

    QString ipAddress;
    int port;

    QString currentTaskId;

    AircraftInfo()
        :port(0)
    {}
};

//devices表

namespace DeviceType {
    enum Type {
        Unknown = 0,
        CentralMaintenanceComputer = 1, // 中央维护计算机
        FlightController = 2,           // 飞控设备
        DataLink = 3,                   // 数传
        VideoTransmitter = 4,           // 图传
        Sensor = 5,                     // 传感器（雷达、相机等）
        Payload = 6                     // 载荷设备
    };

    inline Type fromString(const QString& typeStr) {
        if (typeStr == "FlightController") return FlightController;
        if (typeStr == "CentralMaintenanceComputer") return CentralMaintenanceComputer;
        if (typeStr == "DataLink") return DataLink;
        if (typeStr == "VideoTransmitter") return VideoTransmitter;
        if (typeStr == "Sensor") return Sensor;
        if (typeStr == "Payload") return Payload;
        return Unknown;
    }

    inline QString toString(Type type) {
        switch(type) {
        case FlightController: return "FlightController";
        case CentralMaintenanceComputer: return "CentralMaintenanceComputer";
        case DataLink: return "DataLink";
        case VideoTransmitter: return "VideoTransmitter";
        case Sensor: return "Sensor";
        case Payload: return "Payload";
        default: return "Unknown";
        }
    }
}

struct DeviceInfo {
    QString deviceId;           // 设备ID
    QString deviceName;         // 设备名称
    DeviceType::Type deviceType;// 设备类型
    QString aircraftId;
    QString firmwareVersion;    // 固件版本
    QString hardwareVersion;    // 硬件版本

    bool isOnline;              // 是否在线
    QJsonObject extraInfo;      // 扩展信息（如GPS位置、电量等）

    DeviceInfo()
        : deviceType(DeviceType::Unknown)
        , isOnline(false)
    {}

    bool isValid() const {
        return !deviceId.isEmpty();
    }



    static DeviceInfo fromJson(const QJsonObject& json) {
        DeviceInfo info;
        info.deviceId = json["deviceId"].toString();
        info.deviceName = json["deviceName"].toString();
        info.deviceType = DeviceType::fromString(json["deviceType"].toString());
        info.firmwareVersion = json["firmwareVersion"].toString();
        info.hardwareVersion = json["hardwareVersion"].toString();

        info.isOnline = json["isOnline"].toBool();
        info.extraInfo = json["extraInfo"].toObject();
        return info;
    }

    QJsonObject toJson() const {
        QJsonObject json;
        json["deviceId"] = deviceId;
        json["deviceName"] = deviceName;
        json["deviceType"] = DeviceType::toString(deviceType);
        json["firmwareVersion"] = firmwareVersion;
        json["hardwareVersion"] = hardwareVersion;

        json["isOnline"] = isOnline;
        json["extraInfo"] = extraInfo;
        return json;
    }
};


//files表

namespace FileType {
    enum Type {
        Unknown = 0,
        Firmware,       // 固件文件
        Config,         // 配置文件
        Log,            // 日志文件
        Script          // 脚本文件
    };

    inline Type fromString(const QString& typeStr) {
        if (typeStr == "Firmware") return Firmware;
        if (typeStr == "Config") return Config;
        if (typeStr == "Log") return Log;
        if (typeStr == "Script") return Script;
        return Unknown;
    }

    inline QString toString(Type type) {
        switch(type) {
        case Firmware: return "Firmware";
        case Config: return "Config";
        case Log: return "Log";
        case Script: return "Script";
        default: return "Unknown";
        }
    }
}

struct FileInfo {
    QString fileId;             // 文件ID（主键）
    QString fileName;           // 文件名
    FileType::Type fileType;    // 文件类型
    qint64 fileSize;            // 文件大小（字节）
    QString md5Hash;            // MD5哈希值
    QString storagePath;        // 服务器存储路径
    QString version;            // 版本号（固件文件专用）
    QString description;        // 文件描述
    QString uploader;           // 上传者用户名
    QDateTime uploadTime;       // 上传时间
    int downloadCount;          // 下载次数（统计用）
    bool isActive;              // 是否有效（可被任务使用）

    FileInfo()
        : fileType(FileType::Unknown)
        , fileSize(0)
        , downloadCount(0)
        , isActive(true)
    {}

    bool isValid() const {
        return !fileId.isEmpty() && fileSize > 0;
    }

    bool isFirmware() const {
        return fileType == FileType::Firmware;
    }

    bool isConfig() const {
        return fileType == FileType::Config;
    }

    QString getFileSizeDisplay() const {
        if (fileSize < 1024) return QString("%1 B").arg(fileSize);
        if (fileSize < 1024 * 1024) return QString("%1 KB").arg(fileSize / 1024.0, 0, 'f', 1);
        if (fileSize < 1024 * 1024 * 1024) return QString("%1 MB").arg(fileSize / (1024.0 * 1024.0), 0, 'f', 1);
        return QString("%1 GB").arg(fileSize / (1024.0 * 1024.0 * 1024.0), 0, 'f', 2);
    }

    static FileInfo fromJson(const QJsonObject& json) {
        FileInfo info;
        info.fileId = json["fileId"].toString();
        info.fileName = json["fileName"].toString();
        info.fileType = FileType::fromString(json["fileType"].toString());
        info.fileSize = json["fileSize"].toInteger();
        info.md5Hash = json["md5Hash"].toString();
        info.storagePath = json["storagePath"].toString();
        info.version = json["version"].toString();
        info.description = json["description"].toString();
        info.uploader = json["uploader"].toString();
        info.uploadTime = QDateTime::fromString(json["uploadTime"].toString(), Qt::ISODate);
        info.downloadCount = json["downloadCount"].toInt();
        info.isActive = json["isActive"].toBool(true);
        return info;
    }

    QJsonObject toJson() const {
        QJsonObject json;
        json["fileId"] = fileId;
        json["fileName"] = fileName;
        json["fileType"] = FileType::toString(fileType);
        json["fileSize"] = qint64(fileSize);
        json["md5Hash"] = md5Hash;
        json["storagePath"] = storagePath;
        json["version"] = version;
        json["description"] = description;
        json["uploader"] = uploader;
        json["uploadTime"] = uploadTime.toString(Qt::ISODate);
        json["downloadCount"] = downloadCount;
        json["isActive"] = isActive;
        return json;
    }
};


//tasks表

namespace TaskType {
    enum Type {
        Unknown = 0,
        Upgrade,        // 固件升级
        Config,         // 配置上传
        LogDownload,    // 日志下载
        Diagnostic      // 诊断测试
    };

    inline Type fromString(const QString& typeStr) {
        if (typeStr == "Upgrade") return Upgrade;
        if (typeStr == "Config") return Config;
        if (typeStr == "LogDownload") return LogDownload;
        if (typeStr == "Diagnostic") return Diagnostic;
        return Unknown;
    }

    inline QString toString(Type type) {
        switch(type) {
        case Upgrade: return "Upgrade";
        case Config: return "Config";
        case LogDownload: return "LogDownload";
        case Diagnostic: return "Diagnostic";
        default: return "Unknown";
        }
    }
}
    namespace TaskStatus {
    enum Status {
        Pending = 0,    // 等待执行
        Running,        // 执行中
        Completed,      // 已完成
        Failed,         // 失败
        Cancelled       // 已取消
    };

    inline Status fromString(const QString& statusStr) {
        if (statusStr == "Pending") return Pending;
        if (statusStr == "Running") return Running;
        if (statusStr == "Completed") return Completed;
        if (statusStr == "Failed") return Failed;
        if (statusStr == "Cancelled") return Cancelled;
        return Pending;
    }

    inline QString toString(Status status) {
        switch(status) {
        case Pending: return "Pending";
        case Running: return "Running";
        case Completed: return "Completed";
        case Failed: return "Failed";
        case Cancelled: return "Cancelled";
        default: return "Pending";
        }
    }
}

struct TaskBasicInfo {
    QString taskId;             // 任务ID
    TaskType::Type taskType;    // 任务类型
    QString description;        // 任务描述
    QString fileId;             // 关联的文件ID
    QString aircraftId;
    QString targetDeviceId;     // 目标设备ID
    int priority;               // 优先级（0-10，越大越高）
    TaskStatus::Status status;  // 任务状态
    QDateTime createTime;       // 创建时间
    QDateTime startTime;        // 开始时间
    QDateTime endTime;          // 结束时间
    QString creator;            // 创建者
    QJsonObject parameters;     // 任务参数（扩展用）

    TaskBasicInfo()
        : taskType(TaskType::Unknown)
        , priority(5)
        , status(TaskStatus::Pending)
    {}

    bool isValid() const {
        return !taskId.isEmpty() && taskType != TaskType::Unknown;
    }

    QString getTypeDisplayName() const {
        switch (taskType) {
        case TaskType::Upgrade: return "固件升级";
        case TaskType::Config: return "配置上传";
        case TaskType::LogDownload: return "日志下载";
        case TaskType::Diagnostic: return "诊断测试";
        default: return "未知任务";
        }
    }

    static TaskBasicInfo fromJson(const QJsonObject& json) {
        TaskBasicInfo info;
        info.taskId = json["taskId"].toString();
        info.taskType = TaskType::fromString(json["taskType"].toString());
        info.description = json["description"].toString();
        info.fileId = json["fileId"].toString();
        info.targetDeviceId = json["targetDeviceId"].toString();
        info.priority = json["priority"].toInt();
        info.status = TaskStatus::fromString(json["status"].toString());
        info.createTime = QDateTime::fromString(json["createTime"].toString(), Qt::ISODate);
        info.startTime = QDateTime::fromString(json["startTime"].toString(), Qt::ISODate);
        info.endTime = QDateTime::fromString(json["endTime"].toString(), Qt::ISODate);
        info.creator = json["creator"].toString();
        info.parameters = json["parameters"].toObject();
        return info;
    }

    QJsonObject toJson() const {
        QJsonObject json;
        json["taskId"] = taskId;
        json["taskType"] = TaskType::toString(taskType);
        json["description"] = description;
        json["fileId"] = fileId;
        json["targetDeviceId"] = targetDeviceId;
        json["priority"] = priority;
        json["status"] = TaskStatus::toString(status);
        json["createTime"] = createTime.toString(Qt::ISODate);
        json["startTime"] = startTime.toString(Qt::ISODate);
        json["endTime"] = endTime.toString(Qt::ISODate);
        json["creator"] = creator;
        json["parameters"] = parameters;
        return json;
    }
};

#endif // MODELS_H
