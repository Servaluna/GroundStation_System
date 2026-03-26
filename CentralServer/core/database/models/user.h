#ifndef USER_H
#define USER_H

#include <QString>
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>

#include "../Common/models.h"

class User
{
public:
    // 用户认证（登录验证）
    static UserInfo authenticate(const QString& username, const QString& password);

    // 获取用户信息（可选，供其他功能使用）
    static UserInfo getUserById(int userId);
    static UserInfo getUserByUsername(const QString& username);

    // 更新最后登录时间
    static bool updateLastLogin(int userId);

private:
    User() = delete;  // 纯静态类

    // 密码工具
    static QString hashPassword(const QString& password);

};

#endif // USER_H
