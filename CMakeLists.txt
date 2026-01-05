cmake_minimum_required(VERSION 3.14)

project(SentinelMonitor LANGUAGES CXX)

# Bật tính năng tự động sinh code của Qt
set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# [1] THÊM "Sql" VÀO DÒNG NÀY
find_package(Qt6 REQUIRED COMPONENTS Core Concurrent Sql)

add_executable(SentinelMonitor
    main.cpp
    FileScanner.h
    FileScanner.cpp
    DatabaseManager.h DatabaseManager.cpp
)

# [2] THÊM "Qt6::Sql" VÀO DÒNG NÀY
target_link_libraries(SentinelMonitor PRIVATE Qt6::Core Qt6::Concurrent Qt6::Sql)
