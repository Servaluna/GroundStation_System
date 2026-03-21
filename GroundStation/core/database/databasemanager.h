#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QString>
#include <QApplication>
#include <QDir>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

class DatabaseManager
{
public:
    // 数据库连接
    static bool connect();

    static bool isDatabaseInitialized();

    static bool initializeDatabase();

    // 获取数据库连接（供其他模型使用）
    static QSqlDatabase& getDatabase();

    // 错误信息
    static QString lastError();

private:
    DatabaseManager() = delete;  // 纯静态类，不需要实例化

    static QSqlDatabase m_database;
    static QString m_lastError;
};

// class DatabaseManager : public QObject
// {
//     Q_OBJECT
// public:
//     // 单例模式
//     static DatabaseManager& instance();// 唯一实例

//     // 数据库操作
//     bool open();
//     void close();
//     bool isOpen() const;

//     // 事务支持
//     bool beginTransaction();
//     bool commitTransaction();
//     bool rollbackTransaction();

//     // 错误处理
//     QString lastError() const;

//     // 获取数据库连接
//     QSqlDatabase& database();

// private:
//     explicit DatabaseManager(QObject *parent = nullptr);// 私有构造函数
//     ~DatabaseManager();

//     // 禁止复制
//     DatabaseManager(const DatabaseManager&) = delete;
//     DatabaseManager& operator=(const DatabaseManager&) = delete;

//     // 初始化数据库路径
//     void initDatabasePath();

//     // 私有成员变量声明
//     QSqlDatabase m_database;
//     QString m_databasePath;
//     QString m_lastError;

// signals:
// };

#endif // DATABASEMANAGER_H
