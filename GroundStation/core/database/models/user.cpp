#include "user.h"
#include "core/database/databasemanager.h"

#include <QDebug>
#include <QCryptographicHash>
#include <QSqlQuery>

UserInfo User::login(const QString& username, const QString& password)
{
    UserInfo userInfo;

    if (username.isEmpty() || password.isEmpty()) {
        return userInfo;
    }

    QSqlQuery query(DatabaseManager::getDatabase());
    query.prepare("SELECT id, username, role, password_hash FROM users WHERE username = ? AND is_active = 1");
    query.addBindValue(username);

    if (!query.exec() || !query.next()) {
        return userInfo;  // 用户不存在
    }

    // 验证密码
    QString storedHash = query.value("password_hash").toString();
    QString inputHash = hashPassword(password);

    if (storedHash != inputHash) {
        return userInfo;  // 密码错误
    }

    // 登录成功，返回用户信息
    userInfo.id = query.value("id").toInt();
    userInfo.username = query.value("username").toString();
    userInfo.role = query.value("role").toString();

    return userInfo;
}

bool User::exists(const QString& username)
{
    QSqlQuery query(DatabaseManager::getDatabase());
    query.prepare("SELECT COUNT(*) FROM users WHERE username = ?");
    query.addBindValue(username);

    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }

    return false;
}

UserInfo User::getUserInfo(const QString& username)
{
    UserInfo userInfo;

    QSqlQuery query(DatabaseManager::getDatabase());
    query.prepare("SELECT id, username, role FROM users WHERE username = ?");
    query.addBindValue(username);

    if (query.exec() && query.next()) {
        userInfo.id = query.value("id").toInt();
        userInfo.username = query.value("username").toString();
        userInfo.role = query.value("role").toString();
    }

    return userInfo;
}

QString User::hashPassword(const QString& password)
{
    QByteArray hash = QCryptographicHash::hash(
        password.toUtf8(),
        QCryptographicHash::Sha256
        );
    return QString(hash.toHex());
}
