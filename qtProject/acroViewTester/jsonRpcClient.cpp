#include "JsonRpcClient.h"


JsonRpcClient::JsonRpcClient(QObject* parent)
    : QObject(parent), m_socket(new QTcpSocket(this)), m_nextRequestId(1)
{
    connect(m_socket, &QTcpSocket::connected, this, &JsonRpcClient::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &JsonRpcClient::onDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &JsonRpcClient::onReadyRead);
    // Use the new QTcpSocket error signal syntax
    connect(m_socket, &QTcpSocket::errorOccurred, this, &JsonRpcClient::onErrorOccurred);

    m_buffer.clear();
    m_pendingRequests.clear();
}

JsonRpcClient::~JsonRpcClient()
{
    disconnectFromServer();
    // Clean up any remaining timers
   // qDeleteAll(m_pendingRequests);
    m_pendingRequests.clear();
}

void JsonRpcClient::connectToServer(const QString& host, quint16 port)
{
    if (m_socket->state() == QAbstractSocket::UnconnectedState) {
        qDebug() << "JsonRpcClient: Connecting to" << host << ":" << port;
        m_buffer.clear(); // 清空缓冲区以便新连接
        m_socket->connectToHost(host, port);
    }
    else {
        qWarning() << "JsonRpcClient: Already connected or connecting.";
    }
}

void JsonRpcClient::disconnectFromServer()
{
    if (m_socket->state() != QAbstractSocket::UnconnectedState) {
        qDebug() << "JsonRpcClient: Disconnecting...";
        // Abort connection cleanly
        m_socket->abort(); // Use abort to immediately close and reset
    }
    // Clean up pending requests upon disconnection
    //qDeleteAll(m_pendingRequests); // Deletes timers associated with requests
    m_pendingRequests.clear();
    m_buffer.clear();
}

bool JsonRpcClient::isConnected() const
{
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

qint64 JsonRpcClient::sendRequest(const QString& method, const QJsonValue params,
    std::function<void(const QJsonValue& result)> successCallback,
    std::function<void(int code, const QString& message, const QJsonValue& data)> errorCallback,
    int timeoutMsecs)
{
    if (!isConnected()) {
        qWarning() << "JsonRpcClient: Cannot send request, not connected.";
        return -1;
    }

    qint64 id = generateRequestId();

    QJsonObject requestObj;
    requestObj["jsonrpc"] = CLIENT_JSONRPC_VERSION;
    requestObj["method"] = method;
    if (!params.isNull() && !params.isUndefined()) {
        requestObj["params"] = params;
    }
    requestObj["id"] = id;

    QJsonDocument doc(requestObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

    if (sendPacket(jsonData)) {
        PendingRequest pending;
        pending.id = id;
        pending.method = method;
        pending.successCb = successCallback;
        pending.errorCb = errorCallback;

        // Setup timeout timer
        pending.timer = new QTimer(this);
        pending.timer->setSingleShot(true);
        // Use lambda to capture necessary context for timeout handling
        connect(pending.timer, &QTimer::timeout, this, [this, id]() {
            // Find the request by id and handle timeout
            if (m_pendingRequests.contains(id)) {
                PendingRequest req = m_pendingRequests.take(id);
                qWarning() << "JsonRpcClient: Request" << id << "(method:" << req.method << ") timed out.";
                if (req.errorCb) {
                    req.errorCb(-32000, "Request timed out", QJsonValue()); // Example timeout error
                }
                else {
                    emit errorReceived(id, -32000, "Request timed out", QJsonValue());
                }
                delete req.timer; // Clean up timer
            }
            });
        pending.timer->start(timeoutMsecs);

        m_pendingRequests.insert(id, pending);
        qDebug() << "JsonRpcClient: Sent request id=" << id << "method=" << method;
        return id;
    }
    else {
        return -1;
    }
}

bool JsonRpcClient::sendNotification(const QString& method, const QJsonValue params)
{
    if (!isConnected()) {
        qWarning() << "JsonRpcClient: Cannot send notification, not connected.";
        return false;
    }

    QJsonObject notificationObj;
    notificationObj["jsonrpc"] = CLIENT_JSONRPC_VERSION;
    notificationObj["method"] = method;
    if (!params.isNull() && !params.isUndefined()) {
        notificationObj["params"] = params;
    }
    // No "id" field for notifications

    QJsonDocument doc(notificationObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

    qDebug() << "JsonRpcClient: Sent notification method=" << method;
    return sendPacket(jsonData);
}


bool JsonRpcClient::sendPacket(const QByteArray& jsonData)
{
    if (!isConnected()) return false;

    QByteArray header;
    header.resize(CLIENT_HEADER_LENGTH);
    header.fill(0); // Fill reserved bytes with 0

    // Magic Number (Big Endian)
    quint32 magic = qToBigEndian(CLIENT_MAGIC_NUMBER);
    memcpy(header.data(), &magic, 4);

    // Header Version (Big Endian)
    quint16 version = qToBigEndian(CLIENT_HEADER_VERSION);
    memcpy(header.data() + 4, &version, 2);

    // Payload Length (Big Endian)
    quint32 payloadLength = qToBigEndian(static_cast<quint32>(jsonData.size()));
    memcpy(header.data() + 6, &payloadLength, 4);

    // Combine header and payload
    QByteArray packet = header + jsonData;

    qint64 bytesWritten = m_socket->write(packet);
    if (bytesWritten != packet.size()) {
        qWarning() << "JsonRpcClient: Failed to write entire packet to socket. Written:" << bytesWritten << "Expected:" << packet.size();
        // Consider error handling or retrying logic here
        return false;
    }
    // Ensure data is sent immediately (optional, depends on application needs)
    // m_socket->flush();
    return true;
}

void JsonRpcClient::onConnected()
{
    qDebug() << "JsonRpcClient: Connected to server.";
    emit connected();
}

void JsonRpcClient::onDisconnected()
{
    qWarning() << "JsonRpcClient: Disconnected from server.";
    // Clean up pending requests on disconnect
// qDeleteAll(m_pendingRequests); // Stops and deletes timers
    m_pendingRequests.clear();
    m_buffer.clear(); // Clear buffer on disconnect
    emit disconnected();
}

void JsonRpcClient::onErrorOccurred(QAbstractSocket::SocketError socketErrortemp)
{
    qWarning() << "JsonRpcClient: Socket error:" << m_socket->errorString();
    // Optionally clean up pending requests on critical errors
    // qDeleteAll(m_pendingRequests);
    // m_pendingRequests.clear();
    // m_buffer.clear();
    emit socketError(socketErrortemp);
}

void JsonRpcClient::onReadyRead()
{
    // Append all available data to the buffer
    m_buffer.append(m_socket->readAll());

    // Process buffer for complete packets
    while (true) {
        if (m_buffer.size() < CLIENT_HEADER_LENGTH) {
            // Not enough data for a header yet
            break;
        }

        // Peek at the header without removing it yet
        QByteArray header = m_buffer.left(CLIENT_HEADER_LENGTH);

        // 1. Check Magic Number
        quint32 receivedMagic;
        memcpy(&receivedMagic, header.constData(), 4);
        receivedMagic = qFromBigEndian(receivedMagic);
        if (receivedMagic != CLIENT_MAGIC_NUMBER) {
            qWarning() << "JsonRpcClient: Invalid magic number received. Disconnecting.";
            emit protocolError("Invalid magic number");
            disconnectFromServer();
            return; // Stop processing buffer after disconnect
        }

        // 2. Check Header Version
        quint16 receivedVersion;
        memcpy(&receivedVersion, header.constData() + 4, 2);
        receivedVersion = qFromBigEndian(receivedVersion);
        if (receivedVersion != CLIENT_HEADER_VERSION) {
            qWarning() << "JsonRpcClient: Unsupported header version received:" << receivedVersion << ". Disconnecting.";
            emit protocolError(QString("Unsupported header version: %1").arg(receivedVersion));
            disconnectFromServer();
            return; // Stop processing buffer after disconnect
        }

        // 3. Get Payload Length
        quint32 payloadLength;
        memcpy(&payloadLength, header.constData() + 6, 4);
        payloadLength = qFromBigEndian(payloadLength);

        // Check if the complete packet (header + payload) is available
        int totalPacketSize = CLIENT_HEADER_LENGTH + payloadLength;
        if (m_buffer.size() < totalPacketSize) {
            // Need more data for this packet
            break;
        }

        // Extract the JSON payload
        QByteArray payload = m_buffer.mid(CLIENT_HEADER_LENGTH, payloadLength);

        // Remove the processed packet from the buffer
        m_buffer.remove(0, totalPacketSize);

        // Process the extracted JSON payload
        processPayload(payload);
    } // End while loop
}

void JsonRpcClient::processPayload(const QByteArray& payload)
{
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(payload, &parseError);
    QString jsonString = doc.toJson(QJsonDocument::Indented);
    qDebug() << jsonString;
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JsonRpcClient: JSON parse error:" << parseError.errorString() << "Payload:" << payload;
        emit protocolError(QString("JSON parse error: %1").arg(parseError.errorString()));
        // Decide how to handle parse errors, maybe disconnect or just log
        return;
    }

    if (!doc.isObject()) {
        qWarning() << "JsonRpcClient: Received JSON is not an object:" << payload;
        emit protocolError("Received JSON is not an object");
        return;
    }

    QJsonObject obj = doc.object();
    bool permission = 1;
    if (!permission)
    {
        qint64 id = obj["id"].toVariant().toLongLong(); 
        if (obj.contains("params")) {
            // Successful Response
            PendingRequest pending = m_pendingRequests.take(id);
            qDebug() << "JsonRpcClient: Received result for id=" << id;
            if (pending.successCb) {
                pending.successCb(obj["params"]);
            }
            emit responseReceived(id, obj["params"]);
        }
    }
    else
    {
        if (!obj.contains("jsonrpc") || obj["jsonrpc"].toString() != CLIENT_JSONRPC_VERSION) {
            qWarning() << "JsonRpcClient: Received message with missing or invalid 'jsonrpc' version:" << payload;
            emit protocolError("Invalid 'jsonrpc' version in received message");
            return;
        }

        if (obj.contains("id") && !obj["id"].isNull()) 
        { 
            qint64 id = obj["id"].toVariant().toLongLong(); 

            if (m_pendingRequests.contains(id)) {
                PendingRequest pending = m_pendingRequests.take(id);
                pending.timer->stop(); // Stop the timeout timer
                delete pending.timer;  // Clean up the timer object

                if (obj.contains("result")) {
                    // Successful Response
                    qDebug() << "JsonRpcClient: Received result for id=" << id;
                    if (pending.successCb) {
                        pending.successCb(obj["result"]);
                    }
                    emit responseReceived(id, obj["result"]);

                }
                else if (obj.contains("error")) {
                    // Error Response
                    QJsonObject errorObj = obj["error"].toObject();
                    int code = errorObj.value("code").toInt(-1);
                    QString message = errorObj.value("message").toString("Unknown error");
                    QJsonValue data = errorObj.value("data");
                    qWarning() << "JsonRpcClient: Received error for id=" << id << "code=" << code << "message=" << message;
                    if (pending.errorCb) {
                        pending.errorCb(code, message, data);
                    }
                    emit errorReceived(id, code, message, data);
                }
                else {
                    qWarning() << "JsonRpcClient: Received invalid response object (no result or error) for id=" << id << ":" << payload;
                    emit protocolError(QString("Invalid response object for id %1").arg(id));
                    if (pending.errorCb) { // Notify via error callback if response is malformed
                        pending.errorCb(-32603, "Invalid response object received", QJsonValue()); // Internal error code
                    }
                }
            }
            else {
                qWarning() << "JsonRpcClient: Received response for unknown or timed out request id=" << id << ":" << payload;
                // Ignore or log this unexpected response
            }
        }

        else if (obj.contains("result") && obj.value("id").isNull()) 
        { 
            QJsonObject resultObj = obj["result"].toObject();
            if (resultObj.contains("cmd") && resultObj["cmd"].isString()) {
                QString cmd = resultObj["cmd"].toString();
                QJsonObject data = resultObj.value("data").toObject(); 
                qDebug() << "JsonRpcClient: Received server command:" << cmd;
                emit serverCommandReceived(cmd, data);
            }
            else {
                qWarning() << "JsonRpcClient: Received message with null id but invalid 'result' format (expected cmd/data):" << payload;
                emit protocolError("Received malformed server command");
            }

        }

        else if (obj.contains("method"))
        { 
            QString method = obj["method"].toString();
            qDebug() << "JsonRpcClient: Received notification/request with method:" << method;
            if (method == "DeviceDiscovered" || method == "LoadProject" || method == "SiteSocketInit" || method == "ClientDoCmd" || method == "DoCustom")
            {
                if (obj.contains("params") && obj["params"].isObject()) 
                {
                    QJsonObject params = obj["method"].toObject();
                    emit responseReceived(0, obj);
                    
                }
                else {
                    qWarning() << "JsonRpcClient: Missing or invalid 'params' for DeviceDiscovered notification:" << payload;
                    emit protocolError("Missing or invalid params for DeviceDiscovered notification");
                }
            }

        }
        else {
            qWarning() << "JsonRpcClient: Received unknown JSON-RPC message type:" << payload;
            emit protocolError("Received unknown JSON-RPC message type");
        }

    }
}
qint64 JsonRpcClient::generateRequestId()
{
    // Simple incrementing ID. Consider using atomic or UUID for more robustness
    // especially in multi-threaded clients.
    return m_nextRequestId++;
}
