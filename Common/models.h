#ifndef MODELS_H
#define MODELS_H

#include "qjsonobject.h"
#include <QString>
#include <QDateTime>

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

    bool isAdmin() const { return role == "Admin"; }
    bool isEngineer() const { return role == "Engineer"; }
    bool isOperator() const { return role == "Operator"; }

    static UserInfo fromJson(const QJsonObject& json) {
        UserInfo info;
        info.id = json["id"].toInt();
        info.username = json["username"].toString();
        // info.passwordHash = json["passwordHash"].toString();
        info.fullName = json["fullName"].toString();
        info.role = json["role"].toString();
        return info;
    }

    QJsonObject toJson() const {
        QJsonObject json;
        json["id"] = id;
        json["username"] = username;
        json["passwordHash"] = passwordHash;
        json["fullName"] = fullName;
        json["role"] = role;
        return json;
    }

};
#endif // MODELS_H
