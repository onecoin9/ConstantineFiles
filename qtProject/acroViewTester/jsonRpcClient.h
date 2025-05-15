#ifndef JSONRPCCLIENT_H
#define JSONRPCCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QByteArray>
#include <QHash>
#include <QTimer>
#include <functional> // For std::function
#include <QtEndian>
#include <QJsonParseError>
#include <QJsonArray> // Include if params can be arrays
#include <QDebug>     // For logging/debugging
// �������ƥ��ĳ���
static const quint32 CLIENT_MAGIC_NUMBER = 0x4150524F;//MagicNumber ("APRO")
static const quint16 CLIENT_HEADER_VERSION = 1;//Э��汾�ţ�2�ֽ�
static const int CLIENT_HEADER_LENGTH = 32;//MagicNumber��4�ֽڣ�+Э��汾�ţ�2�ֽڣ�+����ID��8�ֽڣ�+���ݳ��ȣ�4�ֽڣ�= 32�ֽ�
static const QString CLIENT_JSONRPC_VERSION = "2.0";//jsonrpc�汾��

class JsonRpcClient : public QObject
{
    Q_OBJECT

public:
    explicit JsonRpcClient(QObject* parent = nullptr);
    virtual ~JsonRpcClient();

    // ���ӵ�������
    void connectToServer(const QString& host, quint16 port);
    // �Ͽ�����
    void disconnectFromServer();
    // ����Ƿ�����
    bool isConnected() const;

    // ���� JSON-RPC ���� (ʹ�ûص�������Ӧ)
    // �������� ID���������ʧ���򷵻� -1
    qint64 sendRequest(const QString& method, const QJsonValue params = QJsonValue(),
        std::function<void(const QJsonValue& result)> successCallback = nullptr,
        std::function<void(int code, const QString& message, const QJsonValue& data)> errorCallback = nullptr,
        int timeoutMsecs = 5000); // ��ʱʱ�䣬Ĭ��5��

// ���� JSON-RPC ֪ͨ (����Ҫ��Ӧ)
    bool sendNotification(const QString& method, const QJsonValue params = QJsonValue());


signals:
    // ����/�Ͽ��ź�
    void connected();
    void disconnected();
    // Socket �����ź�
    void socketError(QAbstractSocket::SocketError socketErrortemp);
    // Э������źţ����磬�յ���Ч֡��
    void protocolError(const QString& errorString);
    // �յ��������������͵����� (��Ӧ�������� SendClientDoCmd)
    void serverCommandReceived(const QString& cmd, const QJsonObject& data);
    // ͨ�õ���Ӧ/�����ź� (��������ûص�)
    void responseReceived(qint64 id, const QJsonValue& result);
    void errorReceived(qint64 id, int code, const QString& message, const QJsonValue& data);


private slots:
    void onConnected();
    void onDisconnected();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);
    void onReadyRead();
    // void onRequestTimeout();

private:
    // ����ӻ����������������� JSON Payload
    void processPayload(const QByteArray& payload);
    // ����ԭʼ���ݰ�
    bool sendPacket(const QByteArray& jsonData);
    // ����Ψһ������ ID
    qint64 generateRequestId();

    // �ڲ��ṹ�壬���ڸ��ٴ������������ص��Ͷ�ʱ��
    struct PendingRequest {
        qint64 id;
        QString method;
        std::function<void(const QJsonValue& result)> successCb;
        std::function<void(int code, const QString& message, const QJsonValue& data)> errorCb;
        QTimer* timer; // ���ڴ���ʱ�Ķ�ʱ��
    };

    QTcpSocket* m_socket;
    QByteArray m_buffer; // ���ջ�����
    qint64 m_nextRequestId; // ������������ ID
    QHash<qint64, PendingRequest> m_pendingRequests; // �洢�����������
};

#endif // JSONRPCCLIENT_H
