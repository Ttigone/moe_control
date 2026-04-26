#include "tcp_client.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

#include "logger.h"

TcpClient::TcpClient(QObject *parent)
    : QObject(parent),
      socket_(new QTcpSocket(this)),
      reconnect_timer_(new QTimer(this)),
      server_address_("192.168.3.16"),
  server_port_(12345),
      connected_(false) {
  connect(socket_, &QTcpSocket::connected, this, &TcpClient::OnConnected);
  connect(socket_, &QTcpSocket::disconnected, this, &TcpClient::OnDisconnected);
  connect(socket_, &QTcpSocket::readyRead, this, &TcpClient::OnReadyRead);
  connect(socket_, &QTcpSocket::errorOccurred, this, &TcpClient::OnError);

  reconnect_timer_->setInterval(5000);  // 5秒重连
  connect(reconnect_timer_, &QTimer::timeout, this, &TcpClient::TryReconnect);
}

TcpClient::~TcpClient() {
  if (socket_->state() == QTcpSocket::ConnectedState) {
    socket_->disconnectFromHost();
  }
}

bool TcpClient::IsConnected() const { return connected_; }

QString TcpClient::ServerAddress() const { return server_address_; }

void TcpClient::SetServerAddress(const QString &address) {
  if (server_address_ != address) {
    server_address_ = address;
    emit ServerAddressChanged();
  }
}

int TcpClient::ServerPort() const { return server_port_; }

void TcpClient::SetServerPort(int port) {
  if (server_port_ != port) {
    server_port_ = port;
    emit ServerPortChanged();
  }
}

void TcpClient::ConnectToServer() {
  if (socket_->state() == QTcpSocket::ConnectedState) {
    emit ConnectionStatusChanged(tr("Already connected"));
    return;
  }

  emit ConnectionStatusChanged(tr("Connecting..."));
  socket_->connectToHost(server_address_, server_port_);
}

void TcpClient::DisconnectFromServer() {
  reconnect_timer_->stop();
  if (socket_->state() == QTcpSocket::ConnectedState) {
    socket_->disconnectFromHost();
  }
}

void TcpClient::SendCommand(const QString &command) {
  if (socket_->state() != QTcpSocket::ConnectedState) {
    emit ErrorOccurred(tr("Not connected to server"));
    return;
  }

  QByteArray data = command.toUtf8() + "\n";
  qint64 written = socket_->write(data);
  if (written == -1) {
    emit ErrorOccurred(tr("Send failed"));
  }
  socket_->flush();
}

void TcpClient::SwitchMode(int mode) {
  QJsonObject json;
  json["cmd"] = "set_mode";
  json["value"] = mode;  // 0:自动 1:手动 2:远程

  QJsonDocument doc(json);
  SendCommand(doc.toJson(QJsonDocument::Compact));
}

void TcpClient::SetAlarmDelay(int seconds) {
  QJsonObject json;
  json["cmd"] = "set_alarm_delay";
  json["value"] = seconds;

  QJsonDocument doc(json);
  SendCommand(doc.toJson(QJsonDocument::Compact));
}

void TcpClient::SetYoloThreshold(double threshold) {
  QJsonObject json;
  json["cmd"] = "set_threshold";
  int value = qBound(1, qRound(threshold * 100.0), 99);
  json["value"] = value;

  QJsonDocument doc(json);
  SendCommand(doc.toJson(QJsonDocument::Compact));
}

void TcpClient::SetYoloDrawThreshold(double threshold) {
  QJsonObject json;
  json["cmd"] = "set_draw_threshold";
  int value = qBound(20, qRound(threshold * 100.0), 95);
  json["value"] = value;

  QJsonDocument doc(json);
  SendCommand(doc.toJson(QJsonDocument::Compact));
}

void TcpClient::SetStreamProfile(int profile) {
  QJsonObject json;
  json["cmd"] = "set_stream_profile";
  if (profile != 1080 && profile != 720 && profile != 480) {
    profile = 1080;
  }
  json["value"] = profile;

  QJsonDocument doc(json);
  SendCommand(doc.toJson(QJsonDocument::Compact));
}

void TcpClient::SetInferInterval(int n) {
  QJsonObject json;
  json["cmd"] = "set_infer_interval";
  if (n < 1) n = 1;
  if (n > 6) n = 6;
  json["value"] = n;

  QJsonDocument doc(json);
  SendCommand(doc.toJson(QJsonDocument::Compact));
}

void TcpClient::SetYoloEnabled(bool enabled) {
  QJsonObject json;
  json["cmd"] = enabled ? "enable_yolo" : "disable_yolo";

  QJsonDocument doc(json);
  SendCommand(doc.toJson(QJsonDocument::Compact));
}

void TcpClient::SetYoloDrawEnabled(bool enabled) {
  QJsonObject json;
  json["cmd"] = "set_yolo_draw";
  json["value"] = enabled ? 1 : 0;

  QJsonDocument doc(json);
  SendCommand(doc.toJson(QJsonDocument::Compact));
}

void TcpClient::TriggerAlarm() {
  QJsonObject json;
  json["cmd"] = "trigger_alarm";

  QJsonDocument doc(json);
  SendCommand(doc.toJson(QJsonDocument::Compact));
}

void TcpClient::StopAlarm() {
  QJsonObject json;
  json["cmd"] = "stop_alarm";

  QJsonDocument doc(json);
  SendCommand(doc.toJson(QJsonDocument::Compact));
}

void TcpClient::StartRecording(int duration) {
  QJsonObject json;
  json["cmd"] = "start_recording";
  json["duration"] = duration;

  QJsonDocument doc(json);
  SendCommand(doc.toJson(QJsonDocument::Compact));
}

void TcpClient::StopRecording() {
  QJsonObject json;
  json["cmd"] = "stop_recording";

  QJsonDocument doc(json);
  SendCommand(doc.toJson(QJsonDocument::Compact));
}

void TcpClient::OnConnected() {
  connected_ = true;
  reconnect_timer_->stop();
  emit ConnectedChanged();
  emit ConnectionStatusChanged(tr("Connected"));
  LOG_INFO(QString("Connected to server: %1:%2")
               .arg(server_address_)
               .arg(server_port_));
}

void TcpClient::OnDisconnected() {
  connected_ = false;
  emit ConnectedChanged();
  emit ConnectionStatusChanged(tr("Disconnected"));
  LOG_WARN("Disconnected from server");

  // 启动重连
  reconnect_timer_->start();
}

void TcpClient::OnReadyRead() {
  while (socket_->canReadLine()) {
    QByteArray line = socket_->readLine();
    QString data = QString::fromUtf8(line).trimmed();

    if (!data.isEmpty()) {
      ParseReceivedData(data);
      emit DataReceived(data);
    }
  }
}

void TcpClient::OnError(QAbstractSocket::SocketError socket_error) {
  Q_UNUSED(socket_error)
  QString error_string = socket_->errorString();
  emit ErrorOccurred(error_string);
  emit ConnectionStatusChanged(tr("Connection Error"));
  LOG_ERROR(QString("Socket error: %1").arg(error_string));
}

void TcpClient::TryReconnect() {
  LOG_DEBUG("Attempting to reconnect...");
  ConnectToServer();
}

void TcpClient::ParseReceivedData(const QString &data) {
  // JSON格式数据解析会在SystemDataModel中处理
  LOG_TRACE(QString("Received: %1").arg(data));
}
