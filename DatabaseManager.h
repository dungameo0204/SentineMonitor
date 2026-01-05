#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include <QMap>
#include <QSqlDatabase>

class DatabaseManager
{
public:
    static DatabaseManager& instance();
    bool init();

    // [UPDATE] Thêm tham số rootPath để lọc dữ liệu theo thư mục
    QMap<QString, QString> getBaseline(const QString& rootPath);

    // [UPDATE] Thêm tham số rootPath để chỉ update khu vực này
    void updateBaseline(const QString& rootPath, const QMap<QString, QString>& snapshots);

private:
    DatabaseManager();
    QSqlDatabase m_db;
};

#endif // DATABASEMANAGER_H
