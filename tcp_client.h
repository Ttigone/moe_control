#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>

class TcpClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ IsConnected NOTIFY ConnectedChanged)
    Q_PROPERTY(QString serverAddress READ ServerAddress WRITE SetServerAddress NOTIFY ServerAddressChanged)
    Q_PROPERTY(int serverPort READ ServerPort WRITE SetServerPort NOTIFY ServerPortChanged)

public:
    explicit TcpClient(QObject *parent = nullptr);
    ~TcpClient();

    bool IsConnected() const;
    QString ServerAddress() const;
    void SetServerAddress(const QString &address);
    int ServerPort() const;
    void SetServerPort(int port);

    // 发送指令
    Q_INVOKABLE void ConnectToServer();
    Q_INVOKABLE void DisconnectFromServer();
    Q_INVOKABLE void SendCommand(const QString &command);
    Q_INVOKABLE void SwitchMode(int mode); // 0:自动 1:手动 2:远程
    Q_INVOKABLE void SetAlarmDelay(int seconds);
    Q_INVOKABLE void SetYoloThreshold(double threshold);
    Q_INVOKABLE void SetYoloDrawThreshold(double threshold);
    Q_INVOKABLE void SetStreamProfile(int profile); // 1080/720/480
    Q_INVOKABLE void SetInferInterval(int n);       // 1..6
    Q_INVOKABLE void SetYoloEnabled(bool enabled);
    Q_INVOKABLE void SetYoloDrawEnabled(bool enabled);
    Q_INVOKABLE void TriggerAlarm();
    Q_INVOKABLE void StopAlarm();
    Q_INVOKABLE void StartRecording(int duration);
    Q_INVOKABLE void StopRecording();

signals:
    void ConnectedChanged();
    void ServerAddressChanged();
    void ServerPortChanged();
    void DataReceived(const QString &data);
    void ErrorOccurred(const QString &error);
    void ConnectionStatusChanged(const QString &status);

private slots:
    void OnConnected();
    void OnDisconnected();
    void OnReadyRead();
    void OnError(QAbstractSocket::SocketError socket_error);
    void TryReconnect();

private:
    void ParseReceivedData(const QString &data);

    QTcpSocket *socket_;
    QTimer *reconnect_timer_;
    QString server_address_;
    int server_port_;
    bool connected_;
    QString received_buffer_;
};

#endif // TCPCLIENT_H
