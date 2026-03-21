#include "databasemanager.h"

const QString DatabaseManager::CONNECTION_NAME = "main_connection";

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject{parent}
    ,m_initialized(false)
{
}

DatabaseManager::~DatabaseManager()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::initialize()
{

    if (m_initialized) {
        qDebug() << "m_initialized:" <<m_initialized;
        return true;
    }

    if (!QSqlDatabase::isDriverAvailable("QMYSQL")) {
        qDebug() << "可用驱动:" << QSqlDatabase::drivers();

        m_lastError = "QMYSQL driver not available";
        qCritical() << m_lastError;
        emit errorOccurred(m_lastError);

        return false;
    }

    m_db = QSqlDatabase::addDatabase("QMYSQL", CONNECTION_NAME);

    m_db.setHostName("localhost");
    m_db.setPort(3306);
    m_db.setDatabaseName("centralserver");
    m_db.setUserName("root");
    m_db.setPassword("root");

    if(!m_db.open()){
        m_lastError = m_db.lastError().text();
        qCritical() << "数据库连接失败:" << m_db.lastError().text();
        emit errorOccurred(m_lastError);
        return false;
    }

    qDebug() << "Files in exe dir:" << QDir(".").entryList();
    qDebug() << "Checking key libraries:";
    qDebug() << "libmysql.dll exists:" << QFile::exists("libmysql.dll");
    qDebug() << "libcrypto-3-x64.dll exists:" << QFile::exists("libcrypto-3-x64.dll");
    qDebug() << "libssl-3-x64.dll exists:" << QFile::exists("libssl-3-x64.dll");
    qDebug() << "================================";

    m_initialized = true;
    qDebug() << "数据库连接成功";
    emit connectionChanged(true);
    return true;
}

QSqlDatabase DatabaseManager::getDatabase() const
{
    return m_db;
}

QString DatabaseManager::lastError() const
{
    return m_lastError;
}

bool DatabaseManager::isConnected() const
{
    return m_initialized && m_db.isOpen();
}
