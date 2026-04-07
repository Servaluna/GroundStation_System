#ifndef IINSTALLMETHOD_H
#define IINSTALLMETHOD_H

#include <QObject>
#include "InstallContext.h"

class IInstallMethod : public QObject
{
    Q_OBJECT
public:
    explicit IInstallMethod(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~IInstallMethod() = default;

    // 启动安装（立即返回，异步执行）
    virtual void startInstall(const InstallContext& context) = 0;

    // 取消安装
    virtual void cancelInstall(const QString& taskId) = 0;

    // 是否正在安装
    virtual bool isInstalling(const QString& taskId) const = 0;

    //获取安装方法名称
    virtual QString methodName() const = 0;
signals:
    //安装进度更新
    void progressUpdated(QString taskId, int percent, QString step);
    //安装完成
    void installFinished(QString taskId, bool success, QString message, QString newVersion);
};

#endif // IINSTALLMETHOD_H
