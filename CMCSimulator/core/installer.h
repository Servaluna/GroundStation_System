#ifndef INSTALLER_H
#define INSTALLER_H

#include <QObject>

#include <QObject>
#include <QMap>
#include "install/IInstallMethod.h"

class Installer : public QObject
{
    Q_OBJECT
public:
    explicit Installer(QObject *parent = nullptr);
    ~Installer();

    bool init(const QString& configPath = QString());
    //安装入口
    void install(const QString& taskId,
                 const QString& targetDeviceId,
                 const QString& filePath);

    void cancelInstall(const QString& taskId);
    bool isInstalling(const QString& taskId) const;
    //注册安装方法
    void registerInstallMethod(const QString& deviceType, IInstallMethod* method);
signals:
    //安装进度（FileReceiver 可转发给地面站）
    void installProgress(QString taskId, int percent, QString step);

    //安装完成（FileReceiver 通过 InstallResult 转发给地面站）
    void installFinished(QString taskId, bool success, QString message);

private slots:
    void onProgressUpdated(QString taskId, int percent, QString step);
    void onMethodFinished(QString taskId, bool success, QString message, QString newVersion);

private:

    bool loadDeviceConfig(const QString& configPath);

private:
    // 设备配置映射
    struct DeviceConfig {
        QString deviceType;
        QString targetPath;
        QString backupPath;
        QStringList preInstallScript;
        QStringList postInstallScript;
        int timeoutMs;
    };

    //根据设备ID获取安装方法
    IInstallMethod* getInstallMethod(const QString& deviceId);

    //构建安装上下文
    InstallContext buildContext(const QString& taskId,
                                const QString& targetDeviceId,
                                const QString& filePath);

    //加载配置文件
    bool loadConfig(const QString& configPath);
    bool loadConfigFromJson(const QJsonObject& json);

    //使用默认配置
    void useDefaultConfig();

    //获取设备配置
    DeviceConfig getDeviceConfig(const QString& deviceId) const;

private:
    // 安装方法映射（deviceType -> method）
    QMap<QString, IInstallMethod*> m_methods;
    IInstallMethod* m_defaultMethod;

    // 设备配置映射（deviceId -> config）
    QMap<QString, DeviceConfig> m_deviceConfigs;

    // 任务到设备的映射
    QMap<QString, QString> m_taskToDevice;

    // 配置文件搜索路径
    QStringList m_configSearchPaths;
};

#endif // INSTALLER_H
