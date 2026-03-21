#include "databasemanager.h"

#define DEBUG_LOCATION qDebug().nospace() \
<< "[" << Q_FUNC_INFO \
       << " at " << __FILE__ << ":" << __LINE__ << "]"

QSqlDatabase DatabaseManager::m_database;    // 静态成员变量需要定义
QString DatabaseManager::m_lastError;

bool DatabaseManager::connect()
{
    // 数据库路径 - 使用相对路径
    QString dbPath = "data/groundstation.db";

    // 打印当前工作目录和完整路径
    qDebug() << "当前工作目录:" << QDir::currentPath();
    qDebug() << "数据库完整路径:" << QDir(dbPath).absolutePath();

    // 确保 data 目录存在
    QDir dataDir("data");
    if (!dataDir.exists()) {
        qWarning() << "数据目录不存在，将在当前目录创建";
        dataDir.mkpath(".");
    }

    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(dbPath);

    if (!m_database.open()) {
        m_lastError = m_database.lastError().text();
        qCritical() << "数据库连接失败:" << m_lastError;
        return false;
    }

    qDebug() << "数据库连接成功:" << dbPath;

    // 检查是否需要初始化（如果users表不存在）
    if (!isDatabaseInitialized()) {
        if (!initializeDatabase()) {
            qCritical() << "数据库初始化失败";
            return false;
        }
    }

    return true;
}

bool DatabaseManager::isDatabaseInitialized()
{
    QSqlQuery query(m_database);
    query.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='users'");
    return query.next();
}

bool DatabaseManager::initializeDatabase()
{
    DEBUG_LOCATION;
    QSqlQuery query(m_database);

    // 创建用户表
    QString createTableSQL =
        "CREATE TABLE users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT UNIQUE NOT NULL,"
        "password_hash TEXT NOT NULL,"
        "role TEXT NOT NULL,"
        "is_active BOOLEAN DEFAULT 1"
        ")";

    if (!query.exec(createTableSQL)) {
        m_lastError = query.lastError().text();
        qCritical() << "创建用户表失败:" << m_lastError;
        return false;
    }

    qDebug() << "用户表创建成功";

    // 插入默认用户（密码123456的SHA256哈希）
    QString insertUsersSQL =
        "INSERT INTO users (username, password_hash, role) VALUES "
        "('operator', '8d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c92', 'Operator'),"
        "('engineer', '8d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c92', 'Engineer'),"
        "('admin', '8d969eef6ecad3c29a3a629280e686cf0c3f5d5a86aff3ca12020c923adc6c92', 'Admin')";

    if (!query.exec(insertUsersSQL)) {
        m_lastError = query.lastError().text();
        qCritical() << "插入默认用户失败:" << m_lastError;
        return false;
    }

    qDebug() << "默认用户创建成功";
    return true;
}

QSqlDatabase& DatabaseManager::getDatabase()
{
    return m_database;
}

QString DatabaseManager::lastError()
{
    return m_lastError;
}
