#include "acroViewTester.h"
#include <QtWidgets/QApplication>
#include "dumpUtil.h"
#include <winsock2.h>
#include <windows.h>
#include <winnt.h>
#include <Dbghelp.h>
#pragma comment( lib, "Dbghelp.lib" )
/// <summary>
/// �����쳣������λ��������ʱ�����dump������鿴
/// </summary>
/// <param name="pException">ϵͳ�������</param>
/// <returns></returns>
LONG ExceptionCapture(EXCEPTION_POINTERS* pException)
{
    //��ǰʱ�䴮
    const int TIMESTRLEN = 32;
    WCHAR timeStr[TIMESTRLEN];
    SYSTEMTIME time;
    GetLocalTime(&time);
    swprintf_s(timeStr, TIMESTRLEN, L"%4d_%02d_%02d_%02d_%02d_%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
    WCHAR strname[MAX_PATH];
    swprintf_s(strname, MAX_PATH, L"acroview_%s.dmp", timeStr);

    //���� Dump �ļ�
    HANDLE hDumpFile = CreateFile(strname, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hDumpFile != INVALID_HANDLE_VALUE)
    {
        //Dump��Ϣ
        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
        dumpInfo.ExceptionPointers = pException;
        dumpInfo.ThreadId = GetCurrentThreadId();
        dumpInfo.ClientPointers = TRUE;
        //д��Dump�ļ�����
        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, nullptr, nullptr);
    }


    //��������Ի����˳�����
    //QMessageBox::critical(nullptr, "������ʾ", QString("��ǰ���������쳣.\n  �쳣�ļ�:%1").arg(QString::fromWCharArray(strname)), QMessageBox::Ok, QMessageBox::Ok);

    return EXCEPTION_EXECUTE_HANDLER;
}


int main(int argc, char *argv[])
{
    //ע���쳣������
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ExceptionCapture);
    DumpUtil::InitDumpHandler(L"./dumps");
    QApplication a(argc, argv);
    acroViewTester w;

    w.show();
    return a.exec();
}
