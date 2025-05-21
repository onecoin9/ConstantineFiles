#include "JsonRpcClient.h" // 包含 JsonRpcClient 类的头文件


JsonRpcClient::JsonRpcClient(QObject *parent) // JsonRpcClient 类的构造函数实现
    : QObject(parent), // 调用基类 QObject 的构造函数，并传递父对象
      m_socket(new QTcpSocket(this)), // 创建一个新的 QTcpSocket 对象，并将当前对象 (JsonRpcClient) 设置为其父对象
      m_nextRequestId(1) // 初始化请求 ID 计数器为 1
{
    connect(m_socket, &QTcpSocket::connected, this, &JsonRpcClient::onConnected); // 连接套接字的 connected 信号到 onConnected 槽函数
    connect(m_socket, &QTcpSocket::disconnected, this, &JsonRpcClient::onDisconnected); // 连接套接字的 disconnected 信号到 onDisconnected 槽函数
    connect(m_socket, &QTcpSocket::readyRead, this, &JsonRpcClient::onReadyRead); // 连接套接字的 readyRead 信号到 onReadyRead 槽函数
    // 使用新的 QTcpSocket 错误信号语法
    connect(m_socket, &QTcpSocket::errorOccurred, this, &JsonRpcClient::onErrorOccurred); // 连接套接字的 errorOccurred 信号到 onErrorOccurred 槽函数

    m_buffer.clear(); // 清空接收缓冲区
    m_pendingRequests.clear(); // 清空待处理请求的哈希表
}

JsonRpcClient::~JsonRpcClient() // JsonRpcClient 类的析构函数实现
{
    disconnectFromServer(); // 调用断开连接函数，确保资源被释放
    // 清理所有剩余的定时器 (注意：PendingRequest 对象在 clear 时不会自动删除 timer 指针，但 QObject 父子关系会处理)
   // qDeleteAll(m_pendingRequests); // (注释掉) 之前可能用于手动删除 PendingRequest 中的 timer，但父子关系通常足够
    m_pendingRequests.clear(); // 再次清空待处理请求列表（以防万一）
}

void JsonRpcClient::connectToServer(const QString &host, quint16 port) // 连接到服务器函数的实现
{
    if (m_socket->state() == QAbstractSocket::UnconnectedState) { // 检查套接字当前是否未连接
        qDebug() << "JsonRpcClient: Connecting to" << host << ":" << port; // 输出调试信息，表示正在尝试连接
        m_buffer.clear(); // 清空接收缓冲区，为新的连接做准备
        m_socket->connectToHost(host, port); // 发起连接到指定的主机和端口
    } else { // 如果套接字不是未连接状态
        qWarning() << "JsonRpcClient: Already connected or connecting."; // 输出警告信息，表示已经连接或正在连接中
    }
}

void JsonRpcClient::disconnectFromServer() // 断开服务器连接函数的实现
{
    if (m_socket->state() != QAbstractSocket::UnconnectedState) { // 检查套接字当前是否不是未连接状态
        qDebug() << "JsonRpcClient: Disconnecting..."; // 输出调试信息，表示正在断开连接
        // 干净地中止连接
        m_socket->abort(); // 使用 abort() 立即关闭套接字并重置状态，丢弃未发送数据
    }
    // 断开连接时清理待处理的请求
    //qDeleteAll(m_pendingRequests); // (注释掉) 删除与请求关联的定时器 (父子关系应处理)
    m_pendingRequests.clear(); // 清空待处理请求的哈希表
    m_buffer.clear(); // 清空接收缓冲区
}

bool JsonRpcClient::isConnected() const // 检查连接状态函数的实现
{
    return m_socket->state() == QAbstractSocket::ConnectedState; // 返回套接字状态是否为已连接
}

qint64 JsonRpcClient::sendRequest(const QString &method, const QJsonValue params, // 发送请求函数的实现
                                  std::function<void(const QJsonValue& result)> successCallback, // 成功回调
                                  std::function<void(int code, const QString& message, const QJsonValue& data)> errorCallback, // 错误回调
                                  int timeoutMsecs) // 超时时间
{
    if (!isConnected()) { // 检查是否已连接
        qWarning() << "JsonRpcClient: Cannot send request, not connected."; // 如果未连接，输出警告
        return -1; // 返回 -1 表示发送失败
    }

    qint64 id = generateRequestId(); // 生成一个唯一的请求 ID

    QJsonObject requestObj; // 创建一个 JSON 对象用于构建请求
    requestObj["jsonrpc"] = CLIENT_JSONRPC_VERSION; // 设置 JSON-RPC 版本
    requestObj["method"] = method; // 设置请求的方法名
    if (!params.isNull() && !params.isUndefined()) { // 如果参数有效且已定义
        requestObj["params"] = params; // 将参数添加到请求对象中
    }
    requestObj["id"] = id; // 设置请求的 ID

    QJsonDocument doc(requestObj); // 将 JSON 对象包装成 JSON 文档
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact); // 将 JSON 文档转换为紧凑格式的字节数组

    if (sendPacket(jsonData)) { // 调用 sendPacket 发送封装好的数据包
        PendingRequest pending; // 创建一个 PendingRequest 结构体实例
        pending.id = id; // 存储请求 ID
        pending.method = method; // 存储方法名
        pending.successCb = successCallback; // 存储成功回调函数
        pending.errorCb = errorCallback; // 存储错误回调函数

        // 设置超时定时器
        pending.timer = new QTimer(this); // 创建一个新的 QTimer 对象，父对象为当前 JsonRpcClient 实例
        pending.timer->setSingleShot(true); // 设置定时器为单次触发
        // 使用 lambda 表达式捕获必要的上下文来处理超时
        connect(pending.timer, &QTimer::timeout, this, [this, id]() { // 连接定时器的 timeout 信号到 lambda 表达式
            // 通过 ID 查找请求并处理超时
            if (m_pendingRequests.contains(id)) { // 检查待处理请求哈希表中是否还存在该 ID (可能已被响应处理)
                PendingRequest req = m_pendingRequests.take(id); // 从哈希表中移除该请求
                qWarning() << "JsonRpcClient: Request" << id << "(method:" << req.method << ") timed out."; // 输出警告信息，表示请求超时
                if (req.errorCb) { // 如果设置了错误回调函数
                    req.errorCb(-32000, "Request timed out", QJsonValue()); // 调用错误回调，传递超时错误信息 (示例错误码)
                } else { // 如果没有设置错误回调
                    emit errorReceived(id, -32000, "Request timed out", QJsonValue()); // 发射通用的 errorReceived 信号
                }
                delete req.timer; // 删除定时器对象，释放资源
            }
        });
        pending.timer->start(timeoutMsecs); // 启动定时器，设置超时时间

        m_pendingRequests.insert(id, pending); // 将待处理请求信息插入到哈希表中
        qDebug() << "JsonRpcClient: Sent request id=" << id << "method=" << method; // 输出调试信息，表示请求已发送
        return id; // 返回请求的 ID
    } else { // 如果 sendPacket 发送失败
        return -1; // 返回 -1 表示发送失败
    }
}

bool JsonRpcClient::sendNotification(const QString &method, const QJsonValue params) // 发送通知函数的实现
{
    if (!isConnected()) { // 检查是否已连接
        qWarning() << "JsonRpcClient: Cannot send notification, not connected."; // 如果未连接，输出警告
        return false; // 返回 false 表示发送失败
    }

    QJsonObject notificationObj; // 创建一个 JSON 对象用于构建通知
    notificationObj["jsonrpc"] = CLIENT_JSONRPC_VERSION; // 设置 JSON-RPC 版本
    notificationObj["method"] = method; // 设置通知的方法名
    if (!params.isNull() && !params.isUndefined()) { // 如果参数有效且已定义
        notificationObj["params"] = params; // 将参数添加到通知对象中
    }
    // 通知没有 "id" 字段

    QJsonDocument doc(notificationObj); // 将 JSON 对象包装成 JSON 文档
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact); // 将 JSON 文档转换为紧凑格式的字节数组

    qDebug() << "JsonRpcClient: Sent notification method=" << method; // 输出调试信息，表示通知已发送
    return sendPacket(jsonData); // 调用 sendPacket 发送封装好的数据包，并返回其结果
}


bool JsonRpcClient::sendPacket(const QByteArray &jsonData) // 发送原始数据包函数的实现
{
    if (!isConnected()) return false; // 如果未连接，直接返回 false

    QByteArray header; // 创建一个字节数组用于存储协议头
    header.resize(CLIENT_HEADER_LENGTH); // 调整头部大小为预定义的长度
    header.fill(0); // 将头部所有字节填充为 0 (用于保留字段)

    // 魔数 (大端序)
    quint32 magic = qToBigEndian(CLIENT_MAGIC_NUMBER); // 将魔数转换为大端字节序
    memcpy(header.data(), &magic, 4); // 将转换后的魔数拷贝到头部的开头 4 个字节

    // 协议头版本 (大端序)
    quint16 version = qToBigEndian(CLIENT_HEADER_VERSION); // 将协议头版本转换为大端字节序
    memcpy(header.data() + 4, &version, 2); // 将转换后的版本号拷贝到头部的第 4 个字节开始的 2 个字节

    // 载荷长度 (大端序)
    quint32 payloadLength = qToBigEndian(static_cast<quint32>(jsonData.size())); // 获取 JSON 数据的长度，并转换为大端字节序的 quint32
    memcpy(header.data() + 6, &payloadLength, 4); // 将转换后的载荷长度拷贝到头部的第 6 个字节开始的 4 个字节

    // 组合头部和载荷
    QByteArray packet = header + jsonData; // 将头部和 JSON 数据拼接成完整的数据包

    qint64 bytesWritten = m_socket->write(packet); // 将完整的数据包写入 TCP 套接字
    if (bytesWritten != packet.size()) { // 检查写入的字节数是否等于数据包的总大小
        qWarning() << "JsonRpcClient: Failed to write entire packet to socket. Written:" << bytesWritten << "Expected:" << packet.size(); // 如果写入不完整，输出警告
        // 这里可以考虑错误处理或重试逻辑
        return false; // 返回 false 表示写入失败
    }
    // 确保数据立即发送 (可选，取决于应用需求)
    // m_socket->flush(); // 调用 flush() 可以尝试立即发送缓冲区中的数据，但通常由操作系统调度
    return true; // 返回 true 表示数据包已成功写入套接字缓冲区
}

void JsonRpcClient::onConnected() // 处理连接成功槽函数的实现
{
    qDebug() << "JsonRpcClient: Connected to server."; // 输出调试信息，表示已连接到服务器
    emit connected(); // 发射 connected 信号，通知其他对象连接已建立
}

void JsonRpcClient::onDisconnected() // 处理断开连接槽函数的实现
{
    qWarning() << "JsonRpcClient: Disconnected from server."; // 输出警告信息，表示已从服务器断开
        // 断开连接时清理待处理的请求
   // qDeleteAll(m_pendingRequests); // (注释掉) 停止并删除定时器 (父子关系应处理)
    m_pendingRequests.clear(); // 清空待处理请求的哈希表
    m_buffer.clear(); // 清空接收缓冲区
    emit disconnected(); // 发射 disconnected 信号，通知其他对象连接已断开
}

void JsonRpcClient::onErrorOccurred(QAbstractSocket::SocketError socketErrortemp) // 处理套接字错误槽函数的实现
{
    qWarning() << "JsonRpcClient: Socket error:" << m_socket->errorString(); // 输出警告信息，包含具体的套接字错误描述
    // 可选地，在发生严重错误时清理待处理请求
    // qDeleteAll(m_pendingRequests);
    // m_pendingRequests.clear();
    // m_buffer.clear();
    emit socketError(socketErrortemp); // 发射 socketError 信号，传递具体的错误代码
}

void JsonRpcClient::onReadyRead() // 处理数据可读槽函数的实现
{
    // 将所有可用数据追加到缓冲区
    m_buffer.append(m_socket->readAll()); // 从套接字读取所有可用的数据并追加到 m_buffer

    // 处理缓冲区中的完整数据包
    while (true) { // 循环处理，因为缓冲区中可能包含多个数据包或不完整的数据包
        if (m_buffer.size() < CLIENT_HEADER_LENGTH) { // 检查缓冲区数据是否足够包含一个完整的协议头
            // 数据还不够一个协议头
            break; // 退出循环，等待更多数据
        }

        // 查看头部数据，但不从缓冲区移除
        QByteArray header = m_buffer.left(CLIENT_HEADER_LENGTH); // 获取缓冲区开头的协议头数据

        // 1. 检查魔数
        quint32 receivedMagic; // 用于存储接收到的魔数
        memcpy(&receivedMagic, header.constData(), 4); // 从头部数据中拷贝前 4 个字节到 receivedMagic
        receivedMagic = qFromBigEndian(receivedMagic); // 将接收到的魔数从大端字节序转换为主机字节序
        if (receivedMagic != CLIENT_MAGIC_NUMBER) { // 检查转换后的魔数是否与预定义的魔数匹配
            qWarning() << "JsonRpcClient: Invalid magic number received. Disconnecting."; // 如果不匹配，输出警告并准备断开连接
            emit protocolError("Invalid magic number"); // 发射协议错误信号
            disconnectFromServer(); // 调用断开连接函数
            return; // 断开连接后停止处理缓冲区
        }

        // 2. 检查协议头版本
        quint16 receivedVersion; // 用于存储接收到的协议头版本
        memcpy(&receivedVersion, header.constData() + 4, 2); // 从头部数据的第 4 个字节开始拷贝 2 个字节到 receivedVersion
        receivedVersion = qFromBigEndian(receivedVersion); // 将接收到的版本号从大端字节序转换为主机字节序
        if (receivedVersion != CLIENT_HEADER_VERSION) { // 检查转换后的版本号是否与预定义的版本号匹配
            qWarning() << "JsonRpcClient: Unsupported header version received:" << receivedVersion << ". Disconnecting."; // 如果不匹配，输出警告并准备断开连接
            emit protocolError(QString("Unsupported header version: %1").arg(receivedVersion)); // 发射协议错误信号，包含不支持的版本号
            disconnectFromServer(); // 调用断开连接函数
            return; // 断开连接后停止处理缓冲区
        }

        // 3. 获取载荷长度
        quint32 payloadLength; // 用于存储接收到的载荷长度
        memcpy(&payloadLength, header.constData() + 6, 4); // 从头部数据的第 6 个字节开始拷贝 4 个字节到 payloadLength
        payloadLength = qFromBigEndian(payloadLength); // 将接收到的载荷长度从大端字节序转换为主机字节序

        // 检查完整的包 (头部 + 载荷) 是否都已在缓冲区中
        int totalPacketSize = CLIENT_HEADER_LENGTH + payloadLength; // 计算完整数据包的总大小
        if (m_buffer.size() < totalPacketSize) { // 检查缓冲区当前大小是否小于所需总大小
            // 这个包还需要更多数据
            break; // 退出循环，等待更多数据
        }

        // 提取 JSON 载荷
        QByteArray payload = m_buffer.mid(CLIENT_HEADER_LENGTH, payloadLength); // 从缓冲区中提取载荷部分的数据

        // 从缓冲区移除已处理的数据包
        m_buffer.remove(0, totalPacketSize); // 从缓冲区的开头移除整个已处理的数据包

        // 处理提取出的 JSON 载荷
        processPayload(payload); // 调用 processPayload 函数处理 JSON 数据
    } // 结束 while 循环
}

void JsonRpcClient::processPayload(const QByteArray &payload) // 处理 JSON 载荷函数的实现
{
    QJsonParseError parseError; // 用于存储 JSON 解析可能发生的错误
    QJsonDocument doc = QJsonDocument::fromJson(payload, &parseError); // 尝试从字节数组解析 JSON 文档

    if (parseError.error != QJsonParseError::NoError) { // 检查解析过程中是否发生错误
        qWarning() << "JsonRpcClient: JSON parse error:" << parseError.errorString() << "Payload:" << payload; // 如果解析失败，输出警告信息和原始载荷
        emit protocolError(QString("JSON parse error: %1").arg(parseError.errorString())); // 发射协议错误信号，包含解析错误信息
        // 决定如何处理解析错误，可能需要断开连接或仅记录日志
        return; // 停止处理此载荷
    }

    if (!doc.isObject()) { // 检查解析得到的 JSON 文档是否是一个对象
        qWarning() << "JsonRpcClient: Received JSON is not an object:" << payload; // 如果不是对象，输出警告
        emit protocolError("Received JSON is not an object"); // 发射协议错误信号
        return; // 停止处理此载荷
    }

    QJsonObject obj = doc.object(); // 获取 JSON 文档中的顶层对象

    // 检查是否是有效的 JSON-RPC 2.0 响应/消息
    if (!obj.contains("jsonrpc") || obj["jsonrpc"].toString() != CLIENT_JSONRPC_VERSION) { // 检查是否存在 "jsonrpc" 字段且其值是否为 "2.0"
        qWarning() << "JsonRpcClient: Received message with missing or invalid 'jsonrpc' version:" << payload; // 如果无效，输出警告
        emit protocolError("Invalid 'jsonrpc' version in received message"); // 发射协议错误信号
        return; // 停止处理此消息
    }

    // 区分响应和服务器发送的命令 (基于服务器的 SendClientDoCmd 结构)
    if (obj.contains("id") && !obj["id"].isNull()) { // 标准响应 (包含非空的 "id" 字段)
        qint64 id = obj["id"].toVariant().toLongLong(); // 获取响应的 ID (假设是数字类型)

        if (m_pendingRequests.contains(id)) { // 检查待处理请求哈希表中是否存在此 ID
            PendingRequest pending = m_pendingRequests.take(id); // 从哈希表中取出并移除对应的待处理请求信息
            pending.timer->stop(); // 停止与此请求关联的超时定时器
            delete pending.timer;  // 删除定时器对象，释放资源

            if (obj.contains("result")) { // 如果响应对象包含 "result" 字段 (成功响应)
                // 成功响应
                qDebug() << "JsonRpcClient: Received result for id=" << id; // 输出调试信息
                if (pending.successCb) { // 如果设置了成功回调函数
                    pending.successCb(obj["result"]); // 调用成功回调，传递结果值
                }
                emit responseReceived(id, obj["result"]); // 发射通用的 responseReceived 信号

            } else if (obj.contains("error")) { // 如果响应对象包含 "error" 字段 (错误响应)
                // 错误响应
                QJsonObject errorObj = obj["error"].toObject(); // 获取错误对象
                int code = errorObj.value("code").toInt(-1); // 获取错误代码 (如果不存在或类型错误，默认为 -1)
                QString message = errorObj.value("message").toString("Unknown error"); // 获取错误消息 (如果不存在或类型错误，默认为 "Unknown error")
                QJsonValue data = errorObj.value("data"); // 获取可选的错误数据
                qWarning() << "JsonRpcClient: Received error for id=" << id << "code=" << code << "message=" << message; // 输出警告信息
                if (pending.errorCb) { // 如果设置了错误回调函数
                    pending.errorCb(code, message, data); // 调用错误回调，传递错误信息
                }
                emit errorReceived(id, code, message, data); // 发射通用的 errorReceived 信号
            } else { // 如果响应对象既不包含 "result" 也不包含 "error"
                qWarning() << "JsonRpcClient: Received invalid response object (no result or error) for id=" << id << ":" << payload; // 输出警告，表示响应格式无效
                emit protocolError(QString("Invalid response object for id %1").arg(id)); // 发射协议错误信号
                if (pending.errorCb) { // 如果有错误回调，也通知它响应格式错误
                    pending.errorCb(-32603, "Invalid response object received", QJsonValue()); // 调用错误回调，使用内部错误码表示无效响应
                }
            }
        } else { // 如果在待处理请求哈希表中找不到对应的 ID
            qWarning() << "JsonRpcClient: Received response for unknown or timed out request id=" << id << ":" << payload; // 输出警告，可能是未知请求或已超时的请求
            // 忽略或记录这个意外的响应
        }
    } else if (obj.contains("result") && obj.value("id").isNull()) { // 可能是服务器发送的命令 (id 为 null，但有 result 字段，根据特定协议)
        QJsonObject resultObj = obj["result"].toObject(); // 获取 result 字段的对象
        if (resultObj.contains("cmd") && resultObj["cmd"].isString()) { // 检查 result 对象中是否包含字符串类型的 "cmd" 字段
            QString cmd = resultObj["cmd"].toString(); // 获取命令字符串
            QJsonObject data = resultObj.value("data").toObject(); // 获取可选的 "data" 对象
            qDebug() << "JsonRpcClient: Received server command:" << cmd; // 输出调试信息，表示收到了服务器命令
            emit serverCommandReceived(cmd, data); // 发射 serverCommandReceived 信号，传递命令和数据
        } else { // 如果 result 对象的格式不符合预期 (缺少 cmd 或类型错误)
            qWarning() << "JsonRpcClient: Received message with null id but invalid 'result' format (expected cmd/data):" << payload; // 输出警告
            emit protocolError("Received malformed server command"); // 发射协议错误信号
        }

    } else if (obj.contains("method")) { // 收到了来自服务器的请求或通知 (不太常见，但协议可能支持)
        QString method = obj["method"].toString(); // 获取方法名
        qDebug() << "JsonRpcClient: Received notification/request with method:" << method; // 输出调试信息

        if (method == "DeviceDiscovered") { // **** 处理特定的 DeviceDiscovered 通知 ****
            if (obj.contains("params") && obj["params"].isObject()) { // 检查是否存在对象类型的 "params" 字段
                QJsonObject params = obj["params"].toObject(); // 获取参数对象
                if (params.contains("device") && params["device"].isObject() && params.contains("ipHop") && params["ipHop"].isString()) { // 检查参数对象是否包含预期的字段和类型
                    QJsonObject deviceData = params["device"].toObject(); // 获取设备数据对象
                    QString ipHop = params["ipHop"].toString(); // 获取 ipHop 字符串
                    // --- 在这里打印 deviceData 的内容 ---
                    qDebug() << "JsonRpcClient: Received DeviceDiscovered notification for ipHop:" << ipHop; // 输出调试信息
                    qDebug() << "  Device Info:" << deviceData; // 打印整个设备信息 JSON 对象
                    // --- 打印结束 ---
                    qDebug() << "JsonRpcClient: Emitting deviceDiscovered for ipHop:" << ipHop; // 输出准备发射信号的信息
                   // emit deviceDiscovered(deviceData, ipHop); // **** (注释掉) 发射 deviceDiscovered 信号 ****
                } else { // 如果参数格式不正确
                    qWarning() << "JsonRpcClient: Invalid 'params' format for DeviceDiscovered notification:" << params; // 输出警告
                    emit protocolError("Invalid params format for DeviceDiscovered notification"); // 发射协议错误信号
                }
            } else { // 如果缺少参数或参数类型不正确
                qWarning() << "JsonRpcClient: Missing or invalid 'params' for DeviceDiscovered notification:" << payload; // 输出警告
                emit protocolError("Missing or invalid params for DeviceDiscovered notification"); // 发射协议错误信号
            }
        }
        else if(method == "ClientDoCmd"){
            if (obj.contains("params") && obj["params"].isObject()) { // 检查是否存在对象类型的 "params" 字段
                QJsonObject params = obj["params"].toObject(); // 获取参数对象
                qDebug()<<"ClientDoCmd:cmd:"<<params["cmd"];
                qDebug()<<"ClientDoCmd:data:"<<params["data"];
            }
        }
        // 这里可以添加对其他服务器发起的 method 的处理逻辑
    } else { // 如果消息类型无法识别 (既不是响应，也不是已知格式的命令或通知)
        qWarning() << "JsonRpcClient: Received unknown JSON-RPC message type:" << payload; // 输出警告
        emit protocolError("Received unknown JSON-RPC message type"); // 发射协议错误信号
    }
}

qint64 JsonRpcClient::generateRequestId() // 生成请求 ID 函数的实现
{
    // 简单的自增 ID。在多线程客户端中，考虑使用原子操作或 UUID 以提高健壮性。
    return m_nextRequestId++; // 返回当前的 ID 计数器值，然后将其加 1
}
