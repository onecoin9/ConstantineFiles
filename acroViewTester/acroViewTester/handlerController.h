#pragma once
#ifndef HANDLERCONTROLLER_H
#define HANDLERCONTROLLER_H

#include <string>
#include "tcphandler.h"

class HandlerController {
public:
    HandlerController();
    ~HandlerController();

    // ��ѯЭ��汾��
    void getVersion();

    // �趨ʹ��
    void setEnable();

    // ��ʼ����
    void lotStart();

    // ��������
    void lotEnd();

    // ��������
    void endTest();

    // ��ȡ������Ϣ
    void sendAlarm();

private:
    bool m_isEnabled;
    bool m_isBatchRunning;
    TcpHandler tcphanlder;
    std::string m_protocolVersion;
};

#endif // HANDLERCONTROLLER_H 