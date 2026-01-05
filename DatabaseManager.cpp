#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <iostream>

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager _instance;
    return _instance;
}

DatabaseManager::DatabaseManager() {}

bool DatabaseManager::init()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("sentinel_data.db");

    if (!m_db.open()) {
        std::cout << "[DB Error] Cannot open database: " << m_db.lastError().text().toStdString() << std::endl;
        return false;
    }

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS file_hashes (filepath TEXT PRIMARY KEY, hash TEXT)");
    return true;
}

// [LOGIC MỚI] Chỉ lấy dữ liệu thuộc về rootPath này
QMap<QString, QString> DatabaseManager::getBaseline(const QString& rootPath)
{
    QMap<QString, QString> data;
    QSqlQuery query;

    // Sử dụng LIKE 'D:/backup_file%' để tìm các file con
    query.prepare("SELECT filepath, hash FROM file_hashes WHERE filepath LIKE :pattern");
    query.bindValue(":pattern", rootPath + "%");
    query.exec();

    while (query.next()) {
        QString path = query.value(0).toString();
        QString hash = query.value(1).toString();
        data[path] = hash;
    }
    return data;
}

// [LOGIC MỚI] Chỉ xóa và ghi đè dữ liệu của rootPath này, giữ nguyên dữ liệu thư mục khác
void DatabaseManager::updateBaseline(const QString& rootPath, const QMap<QString, QString>& snapshots)
{
    QSqlQuery query;

    // 1. CHỈ XÓA dữ liệu cũ của thư mục đang quét
    query.prepare("DELETE FROM file_hashes WHERE filepath LIKE :pattern");
    query.bindValue(":pattern", rootPath + "%");
    query.exec();

    // 2. Insert dữ liệu mới
    m_db.transaction();
    query.prepare("INSERT INTO file_hashes (filepath, hash) VALUES (?, ?)");

    QMapIterator<QString, QString> i(snapshots);
    while (i.hasNext()) {
        i.next();
        query.addBindValue(i.key());
        query.addBindValue(i.value());
        query.exec();
    }
    m_db.commit();
}
