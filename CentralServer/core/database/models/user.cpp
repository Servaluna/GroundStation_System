#include "user.h"
#include "../databasemanager.h"

UserInfo User::authenticate(const QString& username, const QString& password)
{
    UserInfo userInfo;

    if (username.isEmpty() || password.isEmpty()) {
        qWarning() << "认证失败：用户名或密码为空";
        return userInfo;
    }

    // 获取数据库连接
    QSqlDatabase db = DatabaseManager::instance().getDatabase();

    if (!db.isOpen()) {
        qCritical() << "数据库未连接";
        return userInfo;
    }

    // 查询用户
    QSqlQuery query(db);
    query.prepare("SELECT id, username, password_hash, full_name, role, is_active "
                  "FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec()) {
        qCritical() << "查询用户失败:" << query.lastError().text();
        return userInfo;
    }

    if (!query.next()) {
        qDebug() << "用户不存在:" << username;
        return userInfo;
    }

    // 检查账户是否激活
    bool isActive = query.value("is_active").toBool();
    if (!isActive) {
        qWarning() << "账户已禁用:" << username;
        return userInfo;
    }

    // 获取存储的密码哈希
    QString storedHash = query.value("password_hash").toString();

    // 验证密码
    QString inputHash = hashPassword(password);
    if (storedHash != inputHash) {
        qWarning() << "密码错误:" << username;
        return userInfo;
    }

    // 验证成功，填充用户信息
    userInfo.id = query.value("id").toInt();
    userInfo.username = query.value("username").toString();
    userInfo.passwordHash = storedHash;
    userInfo.fullName = query.value("full_name").toString();
    userInfo.role = query.value("role").toString();
    userInfo.isActive = isActive;

    // 更新最后登录时间
    updateLastLogin(userInfo.id);

    qDebug() << "用户认证成功:" << username << "角色:" << userInfo.role;

    return userInfo;
}

UserInfo User::getUserById(int userId)
{
    UserInfo userInfo;

    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    if (!db.isOpen() || userId <= 0) {
        return userInfo;
    }

    QSqlQuery query(db);
    query.prepare("SELECT id, username, full_name, role, is_active "
                  "FROM users WHERE id = :id");
    query.bindValue(":id", userId);

    if (query.exec() && query.next()) {
        userInfo.id = query.value("id").toInt();
        userInfo.username = query.value("username").toString();
        userInfo.fullName = query.value("full_name").toString();
        userInfo.role = query.value("role").toString();
        userInfo.isActive = query.value("is_active").toBool();
    }

    return userInfo;
}

UserInfo User::getUserByUsername(const QString& username)
{
    UserInfo userInfo;

    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    if (!db.isOpen() || username.isEmpty()) {
        return userInfo;
    }

    QSqlQuery query(db);
    query.prepare("SELECT id, username, full_name, role, is_active "
                  "FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (query.exec() && query.next()) {
        userInfo.id = query.value("id").toInt();
        userInfo.username = query.value("username").toString();
        userInfo.fullName = query.value("full_name").toString();
        userInfo.role = query.value("role").toString();
        userInfo.isActive = query.value("is_active").toBool();
    }

    return userInfo;
}

bool User::updateLastLogin(int userId)
{
    if (userId <= 0) return false;

    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    if (!db.isOpen()) return false;

    QSqlQuery query(db);
    query.prepare("UPDATE users SET last_login = CURRENT_TIMESTAMP WHERE id = :id");
    query.bindValue(":id", userId);

    if (!query.exec()) {
        qWarning() << "更新最后登录时间失败:" << query.lastError().text();
        return false;
    }

    return true;
}

QString User::hashPassword(const QString& password)
{
    QByteArray hash = QCryptographicHash::hash(
        password.toUtf8(),
        QCryptographicHash::Sha256
        );
    return QString(hash.toHex());
}
