#ifndef USER_H
#define USER_H

#include <QString>

// 用户信息结构
struct UserInfo {
    int id;
    QString username;
    QString role;

    UserInfo()        : id(-1) {}

    bool isValid() const { return id > 0; }
};

class User
{
public:
    // 登录验证
    static UserInfo login(const QString& username, const QString& password);

    // 检查用户是否存在
    static bool exists(const QString& username);

    // 获取用户信息
    static UserInfo getUserInfo(const QString& username);

private:
    User() = delete;  // 纯静态类

    // 密码工具
    static QString hashPassword(const QString& password);
};

// #include <QObject>
// #include <QDateTime>
// #include <QSqlQuery>

// // 用户信息结构体 - 纯数据容器
// struct UserInfo {
//     int id;
//     QString username;
//     QString passwordHash;
//     QString email;
//     QString role;           // Operator, Engineer, Admin
//     QDateTime createdAt;
//     QDateTime lastLogin;
//     bool isActive;

//     UserInfo();
//     bool isValid() const;

//     // 角色检查方法
//     bool isAdmin() const;
//     bool isEngineer() const;
//     bool isOperator() const;

// };

// // 用户模型类 - 处理业务逻辑
// class User : public QObject
// {
//     Q_OBJECT
// public:
//     // explicit User(QObject *parent = nullptr);
//     static User& instance();

//     // 初始化
//     bool initTable();

//     // 用户认证
//     bool authenticate(const QString& username, const QString& password);
//     UserInfo getUserByUsername(const QString& username);
//     UserInfo getUserById(int userId);

    // // 用户管理
    // bool createUser(const UserInfo& userInfo);
    // bool updateUser(const UserInfo& userInfo);
    // bool deleteUser(int userId);
    // bool changePassword(int userId, const QString& newPassword);

    // // 状态更新
    // bool updateLastLogin(const QString& username);
    // bool setUserActive(int userId, bool active);

    // // 查询
    // bool exists(const QString& username) const;
    // QList<UserInfo> getAllUsers() const;

    // // 工具方法
    // static QString hashPassword(const QString& password);

// signals:
//     void userLoggedIn(const UserInfo& userInfo);
//     void userCreated(const UserInfo& userInfo);
//     void userUpdated(const UserInfo& userInfo);
//     void userDeleted(int userId);

// private:
//     explicit User(QObject* parent = nullptr);

//     // 从数据库查询构建UserInfo
//     UserInfo userInfoFromQuery(const QSqlQuery& query) const;

//     // 执行SQL查询
//     QSqlQuery executeQuery(const QString& sql, const QVariantList& params = QVariantList()) const;

//     // 创建默认用户
//     void createDefaultUsers();

//     // 禁止复制
//     User(const User&) = delete;
//     User& operator=(const User&) = delete;

// };

#endif // USER_H
