#include "acroViewTester.h"
#include <QtWidgets/QApplication>
#include "dumpUtil.h"
#include "sqlite3.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <winsock2.h>
#include <windows.h>
#include <winnt.h>
#include <Dbghelp.h>
#include <QString>
#include <QDebug>

#pragma comment(lib, "Dbghelp.lib")

namespace {
    // 生成当前时间字符串
    std::wstring GetCurrentTimeString() {
        const int TIMESTRLEN = 32;
        WCHAR timeStr[TIMESTRLEN];
        SYSTEMTIME time;
        GetLocalTime(&time);
        swprintf_s(timeStr, TIMESTRLEN, L"%4d_%02d_%02d_%02d_%02d_%02d",
            time.wYear, time.wMonth, time.wDay,
            time.wHour, time.wMinute, time.wSecond);
        return timeStr;
    }

    // 创建dump文件
    void CreateDumpFile(EXCEPTION_POINTERS* pException, const std::wstring& filename) {
        HANDLE hDumpFile = CreateFile(filename.c_str(),
            GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, nullptr);

        if (hDumpFile != INVALID_HANDLE_VALUE) {
            MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
            dumpInfo.ExceptionPointers = pException;
            dumpInfo.ThreadId = GetCurrentThreadId();
            dumpInfo.ClientPointers = TRUE;

            MiniDumpWriteDump(GetCurrentProcess(),
                GetCurrentProcessId(),
                hDumpFile,
                MiniDumpNormal,
                &dumpInfo,
                nullptr,
                nullptr);

            CloseHandle(hDumpFile);
        }
    }

    // 初始化SQLite数据库
    bool InitializeDatabase() {
        sqlite3* sqldb = nullptr;
        int res = sqlite3_open("data/user.db", &sqldb);

        if (res == SQLITE_OK) {
            qDebug() << "Database opened successfully";
            sqlite3_close(sqldb);
            return true;
        }

        qDebug() << "Failed to open database:";
        qDebug() << "Error code:" << sqlite3_errcode(sqldb);
        qDebug() << "Error message:" << sqlite3_errmsg(sqldb);
        return false;
    }
}

// 异常捕获处理函数
LONG WINAPI ExceptionCapture(EXCEPTION_POINTERS* pException) {
    std::wstring timeStr = GetCurrentTimeString();
    std::wstring dumpFilename = L"acroview_" + timeStr + L".dmp";

    CreateDumpFile(pException, dumpFilename);

    return EXCEPTION_EXECUTE_HANDLER;
}

int main(int argc, char *argv[]) {
    try {
        // 注册异常捕获
        SetUnhandledExceptionFilter(ExceptionCapture);
        DumpUtil::InitDumpHandler(L"./dumps");

        // 初始化数据库
        if (!InitializeDatabase()) {
            qDebug() << "Database initialization failed";
            // 可以决定是否继续运行程序
        }

        // 创建并运行应用程序
        QApplication a(argc, argv);
        acroViewTester w;
        w.show();
        return a.exec();
    }
    catch (const std::exception& e) {
        qDebug() << "Unhandled exception:" << e.what();
        return -1;
    }
}
