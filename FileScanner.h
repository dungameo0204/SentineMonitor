#ifndef FILESCANNER_H
#define FILESCANNER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QDir>
#include <QFileInfoList>
#include <QCryptographicHash>
#include <QtConcurrent>
#include <QFuture>
#include <iostream>

class FileScanner : public QObject
{
    Q_OBJECT
public:
    explicit FileScanner(QObject *parent = nullptr);

    // Hàm quét thư mục
    void scanDirectory(const QString &path);

    // Hàm so sánh với dữ liệu cũ (Giả lập)
    void compareSnapshots();

private:
    // Hàm tính Hash của 1 file (Nặng -> Cần tối ưu)
    static QString computeFileHash(const QString &filePath);

    // Lưu trữ: Đường dẫn file -> Mã Hash
    QMap<QString, QString> m_fileSnapshots;
};

#endif // FILESCANNER_H
