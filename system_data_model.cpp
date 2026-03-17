#include "system_data_model.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

SystemDataModel::SystemDataModel(QObject *parent)
    : QObject(parent),
      light_value_(0.0),
      light_status_(tr("Normal")),
      door_status_(tr("Closed")),
      door_open_(false),
      human_status_(tr("No One")),
      human_detect_level_(0),
      alarm_status_(tr("Normal")),
      alarm_active_(false),
      recording_status_(tr("Not Recording")),
      is_recording_(false),
      work_mode_(0),
      work_mode_text_(tr("Auto Mode")),
      alarm_delay_(10),
      yolo_threshold_(0.6) {}

void SystemDataModel::ParseServerData(const QString &json_data) {
  QJsonParseError error;
  QJsonDocument doc = QJsonDocument::fromJson(json_data.toUtf8(), &error);

  if (error.error != QJsonParseError::NoError) {
    qDebug() << "JSON解析错误:" << error.errorString();
    return;
  }

  if (!doc.isObject()) {
    return;
  }

  QJsonObject obj = doc.object();

  // 解析光照值
  if (obj.contains("light")) {
    double new_light = obj["light"].toDouble();
    if (light_value_ != new_light) {
      light_value_ = new_light;
      UpdateLightStatus();
      emit LightValueChanged();
    }
  }

  // 解析门窗状态
  if (obj.contains("door")) {
    bool door_open = obj["door"].toBool();
    if (door_open_ != door_open) {
      UpdateDoorStatus(door_open);
    }
  }

  // 解析人体检测状态
  if (obj.contains("human")) {
    int level = obj["human"].toInt();
    if (human_detect_level_ != level) {
      UpdateHumanStatus(level);
    }
  }

  // 解析报警状态
  if (obj.contains("alarm")) {
    bool active = obj["alarm"].toBool();
    if (alarm_active_ != active) {
      UpdateAlarmStatus(active);
    }
  }

  // 解析录制状态
  if (obj.contains("recording")) {
    bool recording = obj["recording"].toBool();
    if (is_recording_ != recording) {
      UpdateRecordingStatus(recording);
    }
  }

  // 解析工作模式
  if (obj.contains("mode")) {
    int mode = obj["mode"].toInt();
    if (work_mode_ != mode) {
      work_mode_ = mode;
      UpdateWorkModeText();
      emit WorkModeChanged();
    }
  }

  // 解析阈值设置
  if (obj.contains("alarm_delay")) {
    int delay = obj["alarm_delay"].toInt();
    if (alarm_delay_ != delay) {
      alarm_delay_ = delay;
      emit AlarmDelayChanged();
    }
  }

  if (obj.contains("yolo_threshold")) {
    double threshold = obj["yolo_threshold"].toDouble();
    if (yolo_threshold_ != threshold) {
      yolo_threshold_ = threshold;
      emit YoloThresholdChanged();
    }
  }
}

void SystemDataModel::SetWorkMode(int mode) {
  if (work_mode_ != mode && mode >= 0 && mode <= 2) {
    work_mode_ = mode;
    UpdateWorkModeText();
    emit WorkModeChanged();
  }
}

void SystemDataModel::SetAlarmDelay(int seconds) {
  if (alarm_delay_ != seconds && seconds >= 5 && seconds <= 30) {
    alarm_delay_ = seconds;
    emit AlarmDelayChanged();
  }
}

void SystemDataModel::SetYoloThreshold(double threshold) {
  if (yolo_threshold_ != threshold && threshold >= 0.4 && threshold <= 0.8) {
    yolo_threshold_ = threshold;
    emit YoloThresholdChanged();
  }
}

void SystemDataModel::UpdateLightStatus() {
  QString new_status;
  if (light_value_ < 100) {
    new_status = tr("Dark");
  } else if (light_value_ < 500) {
    new_status = tr("Normal");
  } else {
    new_status = tr("Bright");
  }

  if (light_status_ != new_status) {
    light_status_ = new_status;
    emit LightStatusChanged();
  }
}

void SystemDataModel::UpdateDoorStatus(bool open) {
  door_open_ = open;
  door_status_ = open ? tr("Open") : tr("Closed");
  emit DoorOpenChanged();
  emit DoorStatusChanged();
}

void SystemDataModel::UpdateHumanStatus(int level) {
  human_detect_level_ = level;

  switch (level) {
    case 0:
      human_status_ = tr("No One");
      break;
    case 1:
      human_status_ = tr("Suspected");
      break;
    case 2:
      human_status_ = tr("Confirmed");
      break;
    default:
      human_status_ = tr("Unknown");
      break;
  }

  emit HumanDetectLevelChanged();
  emit HumanStatusChanged();
}

void SystemDataModel::UpdateAlarmStatus(bool active) {
  alarm_active_ = active;
  alarm_status_ = active ? tr("Alarming") : tr("Normal");
  emit AlarmActiveChanged();
  emit AlarmStatusChanged();
}

void SystemDataModel::UpdateRecordingStatus(bool recording) {
  is_recording_ = recording;
  recording_status_ = recording ? tr("Recording") : tr("Not Recording");
  emit IsRecordingChanged();
  emit RecordingStatusChanged();
}

void SystemDataModel::UpdateWorkModeText() {
  switch (work_mode_) {
    case 0:
      work_mode_text_ = tr("Auto Mode");
      break;
    case 1:
      work_mode_text_ = tr("Manual Mode");
      break;
    case 2:
      work_mode_text_ = tr("Remote Mode");
      break;
    default:
      work_mode_text_ = tr("Unknown Mode");
      break;
  }
  emit WorkModeTextChanged();
}
