#include "FileScanner.h"
#include "DatabaseManager.h"
#include <QElapsedTimer>
#include <QDirIterator>
#include <QtConcurrent>
#include <iostream>

FileScanner::FileScanner(QObject *parent) : QObject(parent) {}

// Hàm tính Hash SHA-256 (Tối ưu bộ nhớ)
QString FileScanner::computeFileHash(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return "";
    }

    QCryptographicHash hash(QCryptographicHash::Sha256);
    const int bufferSize = 8192; // 8KB buffer
    char buffer[bufferSize];

    while (!file.atEnd()) {
        qint64 bytesRead = file.read(buffer, bufferSize);
        if (bytesRead > 0) {
            hash.addData(buffer, bytesRead);
        }
    }

    return QString(hash.result().toHex());
}

// Hàm quét thư mục chính
void FileScanner::scanDirectory(const QString &path)
{
    // 1. Kết nối Database trước
    if (!DatabaseManager::instance().init()) {
        std::cout << "[Error] Could not initialize database." << std::endl;
        return;
    }

    std::cout << "[*] Scanning directory: " << path.toStdString() << "..." << std::endl;
    QElapsedTimer timer;
    timer.start();

    // 2. Quét file đệ quy (Recursive) - Lấy tất cả file con bên trong
    QDirIterator it(path, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    QList<QString> filePaths;

    while (it.hasNext()) {
        filePaths.append(it.next());
    }

    if (filePaths.isEmpty()) {
        std::cout << "[!] No files found in this directory." << std::endl;
        return;
    }

    std::cout << "[*] Detected " << filePaths.size() << " files. Computing SHA-256 (Multi-threaded)..." << std::endl;

    // 3. Tính Hash đa luồng (Chạy song song)
    QFuture<QString> future = QtConcurrent::mapped(filePaths, &FileScanner::computeFileHash);
    future.waitForFinished();

    // Gom kết quả hiện tại vào Map: [Đường dẫn] -> [Hash Mới]
    QMap<QString, QString> currentSnapshots;
    int index = 0;
    for (const QString &hash : future) {
        // Chỉ lưu hash nếu file đọc thành công (hash không rỗng)
        if (!hash.isEmpty()) {
            currentSnapshots[filePaths[index]] = hash;
        }
        index++;
    }

    // 4. QUAN TRỌNG: Lấy dữ liệu cũ TƯƠNG ỨNG với thư mục này từ Database
    // Truyền 'path' vào để DatabaseManager chỉ lọc ra các file thuộc thư mục này
    QMap<QString, QString> oldSnapshots = DatabaseManager::instance().getBaseline(path);

    if (oldSnapshots.isEmpty()) {
        // TRƯỜNG HỢP 1: Lần đầu quét thư mục này (Chưa có trong DB)
        std::cout << "\n[!] FIRST RUN DETECTED FOR: " << path.toStdString() << std::endl;
        std::cout << "[*] Saving baseline..." << std::endl;

        // Lưu dữ liệu mới vào DB (Kèm theo path để khoanh vùng update)
        DatabaseManager::instance().updateBaseline(path, currentSnapshots);

        std::cout << "[OK] Baseline saved. This directory is now monitored." << std::endl;
    }
    else {
        // TRƯỜNG HỢP 2: Đã có dữ liệu -> SO SÁNH (Integrity Check)
        std::cout << "\n===================================================" << std::endl;
        std::cout << "           INTEGRITY CHECK REPORT                  " << std::endl;
        std::cout << "===================================================" << std::endl;

        int violations = 0;

        // A. Tìm file bị thay đổi (Modified) hoặc Mới (New)
        QMapIterator<QString, QString> i(currentSnapshots);
        while (i.hasNext()) {
            i.next();
            QString filePath = i.key();
            QString currentHash = i.value();

            if (!oldSnapshots.contains(filePath)) {
                std::cout << "[NEW FILE] " << filePath.toStdString() << std::endl;
                violations++;
            } else {
                QString oldHash = oldSnapshots.value(filePath);
                if (currentHash != oldHash) {
                    std::cout << "[MODIFIED] " << filePath.toStdString() << std::endl;
                    // In ngắn hash để dễ nhìn
                    std::cout << "    Old: " << oldHash.toStdString().substr(0, 10) << "..." << std::endl;
                    std::cout << "    New: " << currentHash.toStdString().substr(0, 10) << "..." << std::endl;
                    violations++;
                }
            }
        }

        // B. Tìm file bị xóa (Deleted)
        // Duyệt qua danh sách cũ, nếu file nào không còn trong danh sách mới -> Đã bị xóa
        QMapIterator<QString, QString> old(oldSnapshots);
        while (old.hasNext()) {
            old.next();
            // old.key() là đường dẫn file cũ
            if (!currentSnapshots.contains(old.key())) {
                std::cout << "[DELETED]  " << old.key().toStdString() << std::endl;
                violations++;
            }
        }

        std::cout << "---------------------------------------------------" << std::endl;
        if (violations == 0) {
            std::cout << "[SECURE] No changes detected. System Integrity Verified." << std::endl;
        } else {
            std::cout << "[WARNING] Found " << violations << " changes in directory structure!" << std::endl;

            // Tự động cập nhật lại trạng thái mới vào Database để lần sau không báo lỗi nữa
            std::cout << "[*] Updating database with new state..." << std::endl;
            DatabaseManager::instance().updateBaseline(path, currentSnapshots);
        }
    }

    std::cout << "\n[Done] Processed in " << timer.elapsed() << " ms." << std::endl;
}
