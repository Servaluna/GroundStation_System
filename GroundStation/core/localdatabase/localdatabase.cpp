#include "localdatabase.h"

#include <QDir>
#include <QSqlError>
#include <QSqlQuery>

LocalDatabase* LocalDatabase::m_instance = nullptr;     //静态成员行外初始化

LocalDatabase* LocalDatabase::getInstance()
{
    if (!m_instance) {
        m_instance = new LocalDatabase();
    }
    return m_instance;
}

void LocalDatabase::destroyInstance()
{
    if (m_instance) {
        delete m_instance;
        m_instance = nullptr;
    }
}

LocalDatabase::LocalDatabase(QObject *parent)
    : QObject(parent)
    , m_isInitialized(false)
{}
LocalDatabase::~LocalDatabase()
{
    close();
}

bool LocalDatabase::init(const QString& dbPath)
{
    if (m_isInitialized) {
        return true;
    }

    m_dbPath = dbPath;

    // 确保目录存在
    QDir dir;
    QString dirPath = QFileInfo(dbPath).absolutePath();
    if (!dir.exists(dirPath)) {
        if (!dir.mkpath(dirPath)) {
            m_lastError = "无法创建数据库目录: " + dirPath;
            qWarning() << m_lastError;
            return false;
        }
    }

    // 创建数据库连接
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbPath);                    //  告诉 Qt 数据库文件在哪里（关键！）路径就是数据库名 SQLite 没有服务器，数据库就是一个文件，路径就是文件位置,使用绝对路径或 Qt 标准路径，明确指定文件名和位置

    if (!m_db.open()) {
        m_lastError = m_db.lastError().text();
        qWarning() << "打开数据库失败:" << m_lastError;
        return false;
    }

    // // 创建表结构
    // if (!createTables()) {
    //     m_lastError = "创建表失败";
    //     return false;
    // }

    m_isInitialized = true;
    qDebug() << "gs_local数据库初始化成功:" << dbPath;
    return true;
}

void LocalDatabase::close()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
    m_isInitialized = false;
}

bool LocalDatabase::executeQuery(const QString& sql)
{
    if (!m_isInitialized) {
        qWarning() << "数据库未初始化";
        return false;
    }

    QSqlQuery query(m_db);
    if (!query.exec(sql)) {
        m_lastError = query.lastError().text();
        qWarning() << "执行SQL失败:" << sql << "错误:" << m_lastError;
        return false;
    }
    return true;
}

QSqlQuery LocalDatabase::executeQueryWithResult(const QString& sql)
{
    QSqlQuery query(m_db);
    if (!m_isInitialized) {
        qWarning() << "数据库未初始化";
        return query;
    }

    if (!query.exec(sql)) {
        m_lastError = query.lastError().text();
        qWarning() << "查询SQL失败:" << sql << "错误:" << m_lastError;
    }
    return query;
}

bool LocalDatabase::beginTransaction()
{
    if (!m_isInitialized) return false;
    return m_db.transaction();
}
bool LocalDatabase::commitTransaction()
{
    if (!m_isInitialized) return false;
    return m_db.commit();

}
bool LocalDatabase::rollbackTransaction()
{
    if (!m_isInitialized) return false;
    return m_db.rollback();
}

// bool LocalDatabase::createTables()
// {
//     // 创建 transferring_tasks 表
//     QString createTableSql = R"(
//         CREATE TABLE IF NOT EXISTS transferring_tasks (
//             id INTEGER PRIMARY KEY AUTOINCREMENT,
//             task_id VARCHAR(64) NOT NULL UNIQUE,
//             file_id VARCHAR(64) NOT NULL,
//             task_type INTEGER NOT NULL DEFAULT 0,
//             description VARCHAR(255) DEFAULT '',
//             target_device_id VARCHAR(64) NOT NULL,
//             priority INTEGER NOT NULL DEFAULT 5,
//             file_name VARCHAR(255) NOT NULL,
//             file_size INTEGER NOT NULL DEFAULT 0,
//             file_md5 VARCHAR(32) NOT NULL,
//             transferred_bytes INTEGER NOT NULL DEFAULT 0,
//             status INTEGER NOT NULL DEFAULT 0,
//             current_step VARCHAR(128) DEFAULT '',
//             error_message TEXT DEFAULT '',
//             create_time INTEGER NOT NULL,
//             start_time INTEGER DEFAULT 0,
//             end_time INTEGER DEFAULT 0,
//             last_update_time INTEGER NOT NULL
//         )
//     )";

//     if (!executeQuery(createTableSql)) {
//         return false;
//     }

//     // 创建索引（可选，提高查询性能）
//     executeQuery("CREATE INDEX IF NOT EXISTS idx_task_id ON transferring_tasks(task_id)");
//     executeQuery("CREATE INDEX IF NOT EXISTS idx_status ON transferring_tasks(status)");
//     executeQuery("CREATE INDEX IF NOT EXISTS idx_priority ON transferring_tasks(priority)");

//     return true;
// }
