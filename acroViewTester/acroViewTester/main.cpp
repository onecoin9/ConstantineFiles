#include "acroViewTester.h"
#include <QtWidgets/QApplication>
#include "dumpUtil.h"
#include <winsock2.h>
#include <windows.h>
#include <winnt.h>
#include <Dbghelp.h>
#pragma comment( lib, "Dbghelp.lib" )
/// <summary>
/// 程序异常捕获，上位机崩溃的时候产生dump，方面查看
/// </summary>
/// <param name="pException">系统捕获参数</param>
/// <returns></returns>
LONG ExceptionCapture(EXCEPTION_POINTERS* pException)
{
    //当前时间串
    const int TIMESTRLEN = 32;
    WCHAR timeStr[TIMESTRLEN];
    SYSTEMTIME time;
    GetLocalTime(&time);
    swprintf_s(timeStr, TIMESTRLEN, L"%4d_%02d_%02d_%02d_%02d_%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
    WCHAR strname[MAX_PATH];
    swprintf_s(strname, MAX_PATH, L"acroview_%s.dmp", timeStr);

    //创建 Dump 文件
    HANDLE hDumpFile = CreateFile(strname, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hDumpFile != INVALID_HANDLE_VALUE)
    {
        //Dump信息
        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
        dumpInfo.ExceptionPointers = pException;
        dumpInfo.ThreadId = GetCurrentThreadId();
        dumpInfo.ClientPointers = TRUE;
        //写入Dump文件内容
        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, nullptr, nullptr);
    }


    //弹出错误对话框并退出程序
    //QMessageBox::critical(nullptr, "错误提示", QString("当前程序遇到异常.\n  异常文件:%1").arg(QString::fromWCharArray(strname)), QMessageBox::Ok, QMessageBox::Ok);

    return EXCEPTION_EXECUTE_HANDLER;
}


int main(int argc, char *argv[])
{
    //注冊异常捕获函数
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ExceptionCapture);
    DumpUtil::InitDumpHandler(L"./dumps");
    QApplication a(argc, argv);
    acroViewTester w;

    w.show();
    return a.exec();
}
