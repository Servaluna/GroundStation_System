#ifndef FILEPROTOCOL_H
#define FILEPROTOCOL_H

#include <QString>
#include <QFile>
#include <QCryptographicHash>

// 文件块大小：4KB（便于演示）
const int CHUNK_SIZE = 4096;

// 计算文件MD5
inline QString calcMD5(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return "";

    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(&file);
    file.close();
    return hash.result().toHex();
}

// 计算数据MD5
inline QString calcDataMD5(const QByteArray& data) {
    return QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex();
}

#endif // FILEPROTOCOL_H
