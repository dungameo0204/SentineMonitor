#include <QCoreApplication>
#include <QTextStream> // Dùng cái này thay cho iostream để hỗ trợ Tiếng Việt tốt hơn
#include <QDir>
#include "FileScanner.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

#ifdef Q_OS_WIN
    // 1. Cài đặt Output là UTF-8 (Để in ra tiếng Việt)
    SetConsoleOutputCP(65001);
    // 2. Cài đặt Input là UTF-8 (QUAN TRỌNG: Để NHẬP được tiếng Việt)
    SetConsoleCP(65001);
#endif

    // Sử dụng QTextStream để đọc/ghi Unicode chuẩn Qt
    QTextStream qin(stdin);
    QTextStream qout(stdout);

    // Đặt encoding cho stream là UTF-8
    qin.setEncoding(QStringConverter::Utf8);
    qout.setEncoding(QStringConverter::Utf8);

    qout << "=== SENTINEL: FILE INTEGRITY MONITOR ===\n";
    qout << "Enter directory to scan: " << Qt::flush; // Qt::flush để đẩy chữ ra màn hình ngay

    // Đọc cả dòng (xử lý được dấu cách)
    QString path = qin.readLine();

    // --- XỬ LÝ ĐƯỜNG DẪN THÔNG MINH ---

    // 1. Xóa dấu ngoặc kép " (Nếu bạn copy path kiểu "Copy as path" trên Win 11)
    path.remove("\"");

    // 2. Chuyển dấu gạch ngược \ thành / (Chuẩn Qt/C++)
    path.replace("\\", "/");

    // 3. Xóa khoảng trắng thừa ở đầu/cuối
    path = path.trimmed();

    // ----------------------------------

    // Nếu người dùng lười không nhập, lấy thư mục hiện tại
    if (path.isEmpty()) {
        path = QDir::currentPath();
    }

    // Chạy Scanner
    FileScanner scanner;
    scanner.scanDirectory(path);

    qout << "=== Press Enter to Exit ===\n" << Qt::flush;
    qin.readLine();

    return 0;
}
