#ifndef JSONRPCCLIENT_H // 如果宏 JSONRPCCLIENT_H 未定义
#define JSONRPCCLIENT_H // 定义宏 JSONRPCCLIENT_H，防止头文件被重复包含

#include <QObject> // 包含 Qt 对象模型基类
#include <QTcpSocket> // 包含 TCP 套接字类
#include <QJsonDocument> // 包含 JSON 文档处理类
#include <QJsonObject> // 包含 JSON 对象类
#include <QJsonValue> // 包含 JSON 值类
#include <QByteArray> // 包含字节数组类
#include <QHash> // 包含哈希表类，用于存储待处理请求
#include <QTimer> // 包含定时器类，用于请求超时
#include <functional> // 包含函数对象库，用于回调函数
#include <QtEndian> // 包含字节序转换函数
#include <QJsonParseError> // 包含 JSON 解析错误类
#include <QJsonArray> // 包含 JSON 数组类 (如果参数可能是数组)
#include <QDebug>     // 包含调试输出功能

// 与服务器匹配的常量
static const quint32 CLIENT_MAGIC_NUMBER = 0x4150524F; // 定义客户端协议的魔数 (大端序 "APRO")
static const quint16 CLIENT_HEADER_VERSION = 1; // 定义客户端协议头的版本号
static const int CLIENT_HEADER_LENGTH = 32; // 定义客户端协议头的固定长度（字节）
static const QString CLIENT_JSONRPC_VERSION = "2.0"; // 定义客户端使用的 JSON-RPC 版本字符串

class JsonRpcClient : public QObject // 定义 JsonRpcClient 类，继承自 QObject 以使用信号和槽
{
    Q_OBJECT // 启用元对象特性，如信号和槽

public: // 公共成员
    explicit JsonRpcClient(QObject *parent = nullptr); // 构造函数，接受一个可选的父对象指针
    virtual ~JsonRpcClient(); // 虚析构函数，确保派生类能正确析构

    // 连接到服务器
    void connectToServer(const QString &host, quint16 port); // 连接到指定主机和端口的服务器
    // 断开连接
    void disconnectFromServer(); // 从服务器断开连接
    // 检查是否连接
    bool isConnected() const; // 返回当前是否已连接到服务器

    // 发送 JSON-RPC 请求 (使用回调处理响应)
    // 返回请求 ID，如果发送失败则返回 -1
    qint64 sendRequest(const QString &method, const QJsonValue params = QJsonValue(), // 发送 JSON-RPC 请求，指定方法名和参数
                       std::function<void(const QJsonValue& result)> successCallback = nullptr, // 成功响应的回调函数
                       std::function<void(int code, const QString& message, const QJsonValue& data)> errorCallback = nullptr, // 错误响应的回调函数
                       int timeoutMsecs = 10000); // 请求超时时间（毫秒），默认 10000 毫秒

    // 发送 JSON-RPC 通知 (不需要响应)
    bool sendNotification(const QString &method, const QJsonValue params = QJsonValue()); // 发送 JSON-RPC 通知，指定方法名和参数


signals: // 信号定义
    // 连接/断开信号
    void connected(); // 当成功连接到服务器时发射
    void disconnected(); // 当与服务器断开连接时发射
    // Socket 错误信号
    void socketError(QAbstractSocket::SocketError socketErrortemp); // 当底层 TCP 套接字发生错误时发射
    // 协议错误信号（例如，收到无效帧）
    void protocolError(const QString &errorString); // 当接收到不符合协议的数据时发射
    // 收到服务器主动发送的命令 (对应服务器的 SendClientDoCmd)
    void serverCommandReceived(const QString &cmd, const QJsonObject &data); // 当收到服务器推送的命令时发射
    // 通用的响应/错误信号 (如果不想用回调)
    void responseReceived(qint64 id, const QJsonValue& result); // 当收到成功的 JSON-RPC 响应时发射（如果未使用回调）
    void errorReceived(qint64 id, int code, const QString& message, const QJsonValue& data); // 当收到错误的 JSON-RPC 响应时发射（如果未使用回调）


private slots: // 私有槽函数
    void onConnected(); // 处理套接字连接成功的槽函数
    void onDisconnected(); // 处理套接字断开连接的槽函数
    void onErrorOccurred(QAbstractSocket::SocketError socketError); // 处理套接字错误的槽函数
    void onReadyRead(); // 处理套接字有新数据可读的槽函数
   // void onRequestTimeout(); // (注释掉) 请求超时的槽函数 (现在使用 lambda 实现)

private: // 私有成员
    // 处理从缓冲区解析出的完整 JSON Payload
    void processPayload(const QByteArray &payload); // 处理接收到的完整 JSON 消息体
    // 发送原始数据包
    bool sendPacket(const QByteArray &jsonData); // 将 JSON 数据封装成协议包并发送
    // 生成唯一的请求 ID
    qint64 generateRequestId(); // 生成用于 JSON-RPC 请求的唯一 ID

    // 内部结构体，用于跟踪待处理的请求及其回调和定时器
    struct PendingRequest { // 定义一个结构体来存储待处理请求的信息
        qint64 id; // 请求的 ID
        QString method; // 请求的方法名
        std::function<void(const QJsonValue& result)> successCb; // 成功回调函数
        std::function<void(int code, const QString& message, const QJsonValue& data)> errorCb; // 错误回调函数
        QTimer* timer; // 指向关联的超时定时器的指针
    };

    QTcpSocket *m_socket; // 指向 TCP 套接字对象的指针
    QByteArray m_buffer; // 用于存储接收到的不完整数据的缓冲区
    qint64 m_nextRequestId; // 用于生成下一个请求 ID 的计数器
    QHash<qint64, PendingRequest> m_pendingRequests; // 存储所有待处理请求的哈希表，键是请求 ID
};

#endif // JSONRPCCLIENT_H // 结束 #ifndef 条件编译
