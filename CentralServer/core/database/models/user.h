#ifndef USER_H
#define USER_H

#include <QString>
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>

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

    // 角色判断
    bool isAdmin() const { return role == "Admin"; }
    bool isEngineer() const { return role == "Engineer"; }
    bool isOperator() const { return role == "Operator"; }
};

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
