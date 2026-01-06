# Sentinel Monitor - File Integrity Monitor (FIM)

A high-performance File Integrity Monitoring tool written in C++ using Qt 6 Framework. This tool detects unauthorized file modifications, deletions, or new file creations in real-time.

## 🔥 Key Features

* **Multi-threaded Hashing:** Uses `QtConcurrent` to calculate SHA-256 hashes in parallel, ensuring high speed for large directories.
* **Stateful Monitoring:** Stores file snapshots in a local SQLite database (`sentinel_data.db`).
* **Change Detection:** Instantly identifies:
    * [MODIFIED] Content changes (Hash mismatch).
    * [NEW] Newly created files.
    * [DELETED] Missing files.
* **Multi-Tenancy:** Supports monitoring multiple distinct directories without data conflict.
* **Memory Optimized:** Streaming API for hashing allows processing large files (10GB+) with minimal RAM usage.

## 🛠 Tech Stack

* **Language:** C++17
* **Framework:** Qt 6.10 (Core, Concurrent, Sql)
* **Build System:** CMake
* **Database:** SQLite

## 🚀 How to Run

1.  Clone the repository.
2.  Open `CMakeLists.txt` in Qt Creator.
3.  Build and Run.
4.  Enter the directory path you want to monitor (e.g., `D:/Projects`).

## 📸 Screenshots

*<img width="1684" height="928" alt="image" src="https://github.com/user-attachments/assets/260704d0-3b8f-41bf-a06a-8fabd58eaa4e" />
*
