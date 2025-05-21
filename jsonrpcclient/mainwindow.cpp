#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog> // 包含文件对话框头文件
#include <QJsonDocument> // 包含 JSON 文档头文件，用于调试输出
#include <QJsonObject> // 包含 JSON 对象头文件
#include <QDebug> // 包含调试输出头文件
#include <QFileDialog>
#include <QFileInfo> // 包含 QFileInfo 头文件用于拆分路径
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // --- Signal/Slot Connections ---
    QObject::connect(&client, &JsonRpcClient::connected, [&]() {
        qDebug() << "Successfully connected!";

        // Example 1: Call GetConfig_Json with callback
        // QJsonObject params_getConfig; // No params needed for this example method
        // client.sendRequest("SiteScanAndConnect", params_getConfig,
        //                    // Success Callback
        //                    [](const QJsonValue& result) {
        //                        qDebug() << "SiteScanAndConnect Success:" << QJsonDocument(result.toObject()).toJson(QJsonDocument::Indented);
        //                    },
        //                    // Error Callback
        //                    [](int code, const QString& message, const QJsonValue& data) {
        //                        qWarning() << "SiteScanAndConnect Error:" << code << message << data;
        //                    }
        //                    );

        // // Example 2: Call a method with parameters (assuming method "Add" exists)
        // QJsonArray params_add;
        // params_add.append(5);
        // params_add.append(10);
        // client.sendRequest("DoJob", params_add, // Using positional params
        //                    [](const QJsonValue& result){ qDebug() << "DoJob Result:" << result.toInt(); },
        //                    [](int code, const QString& message, const QJsonValue& data){ qWarning() << "DoJob Error:" << code << message << data;}
        //                    );

        // // Example 3: Send a notification
        // QJsonObject params_log;
        // params_log["level"] = "info";
        // params_log["message"] = "Client connected and sent requests.";
        // client.sendNotification("LogEvent", params_log); // Assuming server handles "LogEvent"

    });

    QObject::connect(&client, &JsonRpcClient::disconnected, [&]() {
        qWarning() << "Disconnected from server.";
        // Maybe try to reconnect or exit
        // a.quit();
    });

    QObject::connect(&client, &JsonRpcClient::socketError, [&](QAbstractSocket::SocketError error) {
        qCritical() << "Socket Error:" << error;
        // Handle error, maybe retry connection
    });

    QObject::connect(&client, &JsonRpcClient::protocolError, [&](const QString& errorString) {
        qCritical() << "Protocol Error:" << errorString;
        // Handle error, might need to disconnect
    });

    QObject::connect(&client, &JsonRpcClient::serverCommandReceived, [&](const QString& cmd, const QJsonObject& data) {
        qInfo() << "Received Command from Server - Cmd:" << cmd << "Data:" << QJsonDocument(data).toJson(QJsonDocument::Compact);
        // Process server commands like "LogOut", "SetStatus", etc.
        if (cmd == "LogOut") {
            qInfo() << "Server requested logout.";
            // Perform logout actions
            client.disconnectFromServer();
            //a.quit();
        }
    });

    // --- Connect to Server ---
    client.connectToServer("127.0.0.1", 12345); // Replace with your server's IP and port
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_SiteScanAndConnect_clicked()
{
    //Example 1: Call GetConfig_Json with callback
    QJsonObject params_getConfig; // No params needed for this example method
    client.sendRequest("SiteScanAndConnect", params_getConfig,
                       // Success Callback
                       [](const QJsonValue& result) {
                           qDebug() << "SiteScanAndConnect Success:" << QJsonDocument(result.toObject()).toJson(QJsonDocument::Indented);
                       },
                       // Error Callback
                       [](int code, const QString& message, const QJsonValue& data) {
                           qWarning() << "SiteScanAndConnect Error:" << code << message << data;
                       }
                       );
}


void MainWindow::on_loadproject_clicked()
{
    // 打开文件对话框让用户选择文件
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    tr("选择项目文件"), // 对话框标题
                                                    "", // 初始目录 (空表示上次使用的目录或默认目录)
                                                    tr("项目文件 (*.your_project_extension);;所有文件 (*.*)")); // 文件过滤器

    // 检查用户是否选择了文件
    if (!filePath.isEmpty()) {
        qDebug() << "选择的文件路径:" << filePath;

        // 使用 QFileInfo 拆分路径和文件名
        QFileInfo fileInfo(filePath);
        QString path = fileInfo.path(); // 获取路径部分
        QString taskFileName = fileInfo.fileName(); // 获取文件名部分

        qDebug() << "拆分后 - 路径:" << path << "文件名:" << taskFileName;

        // 构造 JSON-RPC 请求参数
        QJsonObject params;
        // params["filePath"] = filePath; // 注释掉原来的发送整个路径
        params["path"] = path; // 添加路径参数
        params["taskFileName"] = taskFileName; // 添加文件名参数

        // 通过 client 发送 LoadProject 请求
        client.sendRequest("LoadProject", params,
                           // 成功回调
                           [filePath](const QJsonValue& result) { // 捕获 filePath 用于日志
                               qDebug() << "LoadProject Success for file:" << filePath << "Result:" << QJsonDocument::fromVariant(result.toVariant()).toJson(QJsonDocument::Indented);
                               // 在这里处理成功加载项目的逻辑，例如更新 UI
                           },
                           // 错误回调
                           [filePath](int code, const QString& message, const QJsonValue& data) { // 捕获 filePath 用于日志
                               qWarning() << "LoadProject Error for file:" << filePath << "Code:" << code << "Message:" << message << "Data:" << data;
                               // 在这里处理加载项目失败的逻辑，例如显示错误消息
                           }
                           );
    } else {
        qDebug() << "用户取消了文件选择。";
    }
}


void MainWindow::on_doJob_clicked()
{

}



void MainWindow::on_getProjectInfo_clicked()
{
    QJsonObject params_getConfig; // No params needed for this example method
    client.sendRequest("GetProjectInfo", params_getConfig,
                       // Success Callback
                       [](const QJsonValue& result) {
                           qDebug() << "GetProjectInfo Success:" << QJsonDocument(result.toObject()).toJson(QJsonDocument::Indented);
                       },
                       // Error Callback
                       [](int code, const QString& message, const QJsonValue& data) {
                           qWarning() << "GetProjectInfo Error:" << code << message << data;
                       }
                       );
}


void MainWindow::on_doCustom_clicked()
{
    // --- 从你的 AngKMainFrame::onSlotStartBurnData 获取这些值 ---
    // 假设你已经有了这些变量的值：
    // std::string iter_first_progIndex = iter.first; // 例如 "BPU0" 或类似的标识
    // std::string param_strIP = mapProgIP[iter.first].strIP;
    // int param_nHopNum = mapProgIP[iter.first].nHopNum;
    // uint32_t param_SktEn = iter.second;
    // std::string docmdSeqJson_std_string = docmdSeqJson; // 这是 ExecuteBurnCommand 生成的 JSON 字符串

    // 示例值 (你需要用实际值替换它们)
    QString progIndex_for_logging = "BPU0_Site1"; // 用于日志/回调的标识
    QString actual_strIP = "192.168.70.109";//填入返回给你的ag06上位机ip
    int actual_nHopNum = 0;
    quint32 actual_SktEn = 0x0000FFFF; // 例如，所有 site 使能
    QJsonObject docmdSeqJson;//= "{"CmdID":"1806","CmdRun":"Read","CmdSequences":[{"ID":"806","Name":"Read"}],"CmdSequencesGroupCnt":1}"; // 实际的 JSON 命令字符串
    docmdSeqJson["CmdID"] = "1806";
    docmdSeqJson["CmdRun"] = "Read";
    QJsonArray CmdSequences;
    QJsonObject cmd;
    cmd["ID"] = "806";
    cmd["Name"] = "Read";
    CmdSequences.append(cmd);
    docmdSeqJson["CmdSequences"] = CmdSequences;
    docmdSeqJson["CmdSequencesGroupCnt"] = 1;
    // --- 构造 JSON-RPC 请求参数 ---
    QJsonObject params;
    params["strIP"] = actual_strIP;
    params["nHopNum"] = actual_nHopNum;
    params["PortID"] = 0;
    params["CmdFlag"] = 0;
    params["CmdID"] = 1047;//这个是区分dojob还是docustom
    params["SKTEn"] = static_cast<double>(actual_SktEn); // JSON 数字通常是 double
    params["BPUID"] = 8;
    params["docmdSeqJson"] = docmdSeqJson;

    // 通过 client 发送 DoCustom 请求
    // 假设 'client' 是你的 JsonRpcClient 实例
    client.sendRequest("DoCustom", params,
                       // 成功回调 (这是对 "DoCustom request accepted" 的响应)
                       [progIndex_for_logging](const QJsonValue& result) {
                           QJsonDocument resultDoc = QJsonDocument::fromVariant(result.toVariant());
                           qDebug() << "DoCustom request accepted for" << progIndex_for_logging << "Result:" << resultDoc.toJson(QJsonDocument::Indented);
                           // 可以在这里更新UI，表示任务已提交
                           // 真正的完成通知会通过 ClientDoCmd (DoCustomCompletion) 异步到达
                       },
                       // 错误回调
                       [progIndex_for_logging](int code, const QString& message, const QJsonValue& data) {
                           qWarning() << "DoCustom request failed for" << progIndex_for_logging << "Code:" << code << "Message:" << message << "Data:" << data;
                           // 在这里处理请求失败的逻辑
                       }
                       );
}




void MainWindow::on_getProjectInfoExt_clicked()
{
    QJsonObject params_getConfig; // No params needed for this example method
    params_getConfig["project_url"] = "C:/Users/Administrator/Desktop/XT422/AIPE/Build/project/default.eapr";
    client.sendRequest("GetProjectInfoExt", params_getConfig,
                       // Success Callback
                       [](const QJsonValue& result) {
                           QJsonObject logOutput;
                           logOutput["status"] = "GetProjectInfoExt Success";
                           // Assuming 'result' is an object as per your original code.
                           // If 'result' could be any QJsonValue type (object, array, string, number, bool, null),
                           // you can directly assign it:
                           // logOutput["data"] = result;
                           // If you are sure it's an object and want to ensure that:
                           if (result.isObject()) {
                               logOutput["data"] = result.toObject();
                           } else if (result.isArray()) {
                               logOutput["data"] = result.toArray();
                           } else {
                               logOutput["data"] = result; // Store as is (string, number, bool, null)
                           }

                           // Use .noquote() to prevent qDebug from adding extra quotes around the JSON string
                           qDebug().noquote() << QJsonDocument(logOutput).toJson(QJsonDocument::Indented);
                       },
                       // Error Callback
                       [](int code, const QString& message, const QJsonValue& data) {
                           qWarning() << "GetProjectInfoExt Error:" << code << message << data;
                       }
                       );
}


void MainWindow::on_getSKTInfo_clicked()
{
    QJsonObject params_getConfig; // No params needed for this example method
    client.sendRequest("GetSKTInfo", params_getConfig,
                       // Success Callback
                       [](const QJsonValue& result) {
                           qDebug() << "GetSKTInfo Success:" << QJsonDocument(result.toObject()).toJson(QJsonDocument::Indented);
                       },
                       // Error Callback
                       [](int code, const QString& message, const QJsonValue& data) {
                           qWarning() << "GetSKTInfo Error:" << code << message << data;
                       }
                       );
}




void MainWindow::on_sendUUID_clicked()
{

    QString uuid = "123";
    // 构造 JSON-RPC 请求参数
    QJsonObject params;
    QString strIP = "192.168.70.109";
    quint32 hopname = 0;
    params["uuid"] = uuid;
    params["strIP"] = strIP;
    params["nHopNum"] = static_cast<double>(hopname);
    quint32 actual_SktEn = 0x0000FFFF; // 例如，所有 site 使能
    params["sktEnable"] = static_cast<double>(actual_SktEn);


    client.sendRequest("SendUUID", params,
                       // 成功回调
                       [](const QJsonValue& result) { // 捕获 filePath 用于日志
                           qDebug() << "SendUUID Success Result:" << QJsonDocument::fromVariant(result.toVariant()).toJson(QJsonDocument::Indented);
                           // 在这里处理成功加载项目的逻辑，例如更新 UI
                       },
                       // 错误回调
                       [](int code, const QString& message, const QJsonValue& data) { // 捕获 filePath 用于日志
                           qWarning() << "SendUUID Error Code:" << code << "Message:" << message << "Data:" << data;
                           // 在这里处理加载项目失败的逻辑，例如显示错误消息
                       }
                       );
}

