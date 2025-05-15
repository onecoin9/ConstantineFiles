#ifndef TESTERDEFINITIONS_H
#define TESTERDEFINITIONS_H
#pragma once

#include <string>
#include <vector>
extern int globalCmdID;
extern QString globalCmdRun;

enum class TesterSubCmdID
{
    UnEnable = 0x000,
    CheckID = 0x400,
    PinCheck = 0x401,
    InsertionCheck = 0x402,
    AutoSensing = 0x403,

    DevicePowerOn = 0x600,
    DevicePowerOff = 0x601,
    PowerOn = 0x602,
    PowerOff = 0x603,

    SubProgram = 0x800,
    SubErase = 0x801,
    SubVerify = 0x802,
    SubBlankCheck = 0x803,
    SubSecure = 0x804,
    SubIllegalCheck = 0x805,
    SubRead = 0x806,
    EraseIfBlankCheckFailed = 0x807,
    LowVerify = 0x808,
    HighVerify = 0x809,
    ChecksumCompare = 0x80A,
    SubReadTesterUID = 0x80B,
    HighLowVerify = 0x80C
};

enum class TesterCmdID
{
    Program = 0x1800,
    Erase = 0x1801,
    Verify = 0x1802,
    BlankCheck = 0x1803,
    Secure = 0x1804,
    IllegalCheck = 0x1805,
    Read = 0x1806,
    ReadTesterUID = 0x1807,
    Custom = 0x1901,
};

struct TesterOperationConfig
{
    TesterCmdID cmdID;
    TesterSubCmdID subCmdID;
    std::string description;
    uint32_t parameters;
};

const std::vector<TesterOperationConfig> TesterOperationConfigs = {
    {TesterCmdID::Program, TesterSubCmdID::SubProgram, "Program operation", 0},
    {TesterCmdID::Erase, TesterSubCmdID::SubErase, "Erase operation", 0},
    {TesterCmdID::Verify, TesterSubCmdID::SubVerify, "Verify operation", 0},
    {TesterCmdID::Read, TesterSubCmdID::SubRead, "Read operation", 0},
    {TesterCmdID::ReadTesterUID, TesterSubCmdID::SubReadTesterUID, "Read Tester UID", 0},
};

struct MainBoardInfo {
    QString hardwareOEM;
    QString hardwareSN;
    QString hardwareUID;
    QString hardwareVersion;
};

struct DeviceInfo {
    int chainID;
    QString dpsFpgaVersion;
    QString dpsFwVersion;
    QString firmwareVersion;
    QString firmwareVersionDate;
    QString fpgaLocation;
    QString fpgaVersion;
    int hopNum;
    QString ip;
    bool isLastHop;
    int linkNum;
    QString mac;
    MainBoardInfo mainBoardInfo;
    QString muAppVersion;
    QString muAppVersionDate;
    QString muLocation;
    QString port;
    QString siteAlias;
    QString ipHop;

    void clear() {
        chainID = 0;
        dpsFpgaVersion.clear();
        dpsFwVersion.clear();
        firmwareVersion.clear();
        firmwareVersionDate.clear();
        fpgaLocation.clear();
        fpgaVersion.clear();
        hopNum = 0;
        ip.clear();
        isLastHop = false;
        linkNum = 0;
        mac.clear();
        mainBoardInfo = MainBoardInfo();
        muAppVersion.clear();
        muAppVersionDate.clear();
        muLocation.clear();
        port.clear();
        siteAlias.clear();
        ipHop.clear();
    }
};

struct ProjectInfo {
    int errorCode;
    QString message;
    QString details;

    void clear() {
        errorCode = 0;
        message.clear();
        details.clear();
    }
};

struct JobResult {
    QString BPUID;
    int SKTIdx;
    int nHopNum;
    QString result;
    QString strip;

    void clear() {
        BPUID.clear();
        SKTIdx = 0;
        nHopNum = 0;
        result.clear();
        strip.clear();
    }
};
#endif