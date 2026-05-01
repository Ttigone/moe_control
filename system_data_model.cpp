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
      recording_elapsed_(0),
      recording_duration_(0),
      work_mode_(0),
      work_mode_text_(tr("Auto Mode")),
      alarm_delay_(10),
      yolo_threshold_(0.6),
      yolo_draw_threshold_(0.4),
      infer_interval_(1),
      stream_fps_(0.0),
      infer_ms_(0.0),
      yolo_enabled_(false),
      yolo_draw_enabled_(true) {}

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

  const QString type = obj.value("type").toString();

  // yolo事件：{"type":"yolo","person":1,"conf":0.81}
  if (type == "yolo") {
    int person = obj.value("person").toInt(0);
    UpdateHumanStatus(person > 0 ? 2 : 0);
    return;
  }

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
    bool door_open = obj["door"].toInt() != 0;
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

  // 当前服务端用 pir: 0=无人 1=疑似 2=确认
  if (obj.contains("pir")) {
    int level = obj["pir"].toInt();
    if (human_detect_level_ != level) {
      UpdateHumanStatus(level);
    }
  }

  // 解析报警状态
  if (obj.contains("alarm")) {
    bool active = obj["alarm"].toInt(0) != 0;
    if (alarm_active_ != active) {
      UpdateAlarmStatus(active);
    }
  }

  // 处理报警事件
  if (type == "alarm" && obj.contains("active")) {
    bool active = obj["active"].toInt(0) != 0;
    if (alarm_active_ != active) {
      UpdateAlarmStatus(active);
    }
  }

  // 解析录制状态
  if (obj.contains("recording")) {
    bool recording = obj["recording"].toInt(0) != 0;
    if (is_recording_ != recording) {
      UpdateRecordingStatus(recording);
    }
  }

  if (obj.contains("active") && type == "recording") {
    bool recording = obj["active"].toInt(0) != 0;
    if (is_recording_ != recording) {
      UpdateRecordingStatus(recording);
    }
  }

  if (obj.contains("elapsed")) {
    int elapsed = obj["elapsed"].toInt();
    if (elapsed < 0) elapsed = 0;
    if (recording_elapsed_ != elapsed) {
      recording_elapsed_ = elapsed;
      emit RecordingElapsedChanged();
    }
  }

  if (obj.contains("record_elapsed")) {
    int elapsed = obj["record_elapsed"].toInt();
    if (elapsed < 0) elapsed = 0;
    if (recording_elapsed_ != elapsed) {
      recording_elapsed_ = elapsed;
      emit RecordingElapsedChanged();
    }
  }

  if (obj.contains("duration")) {
    int duration = obj["duration"].toInt();
    if (duration < 0) duration = 0;
    if (recording_duration_ != duration) {
      recording_duration_ = duration;
      emit RecordingDurationChanged();
    }
  }

  if (obj.contains("record_duration")) {
    int duration = obj["record_duration"].toInt();
    if (duration < 0) duration = 0;
    if (recording_duration_ != duration) {
      recording_duration_ = duration;
      emit RecordingDurationChanged();
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

  // 服务端 state: delay / threshold(整数百分比) / yolo
  if (obj.contains("delay")) {
    int delay = obj["delay"].toInt();
    if (alarm_delay_ != delay) {
      alarm_delay_ = delay;
      emit AlarmDelayChanged();
    }
  }

  if (obj.contains("threshold")) {
    double threshold = obj["threshold"].toInt() / 100.0;
    if (!qFuzzyCompare(yolo_threshold_ + 1.0, threshold + 1.0)) {
      yolo_threshold_ = threshold;
      emit YoloThresholdChanged();
    }
  }

  if (obj.contains("draw_threshold")) {
    double threshold = obj["draw_threshold"].toInt() / 100.0;
    if (!qFuzzyCompare(yolo_draw_threshold_ + 1.0, threshold + 1.0)) {
      yolo_draw_threshold_ = threshold;
      emit YoloDrawThresholdChanged();
    }
  }

  if (obj.contains("infer_interval")) {
    int n = obj["infer_interval"].toInt();
    if (n < 1) n = 1;
    if (n > 6) n = 6;
    if (infer_interval_ != n) {
      infer_interval_ = n;
      emit InferIntervalChanged();
    }
  }

  if (obj.contains("fps")) {
    double fps = obj["fps"].toDouble();
    if (!qFuzzyCompare(stream_fps_ + 1.0, fps + 1.0)) {
      stream_fps_ = fps;
      emit StreamFpsChanged();
    }
  }

  if (obj.contains("infer_ms")) {
    double infer_ms = obj["infer_ms"].toDouble();
    if (!qFuzzyCompare(infer_ms_ + 1.0, infer_ms + 1.0)) {
      infer_ms_ = infer_ms;
      emit InferMsChanged();
    }
  }

  if (obj.contains("yolo")) {
    bool enabled = obj["yolo"].toInt() != 0;
    if (yolo_enabled_ != enabled) {
      yolo_enabled_ = enabled;
      emit YoloEnabledChanged();
    }
  }

  if (obj.contains("yolo_draw")) {
    bool enabled = obj["yolo_draw"].toInt() != 0;
    if (yolo_draw_enabled_ != enabled) {
      yolo_draw_enabled_ = enabled;
      emit YoloDrawEnabledChanged();
    }
  }

  if (obj.contains("yolo_draw_enabled")) {
    bool enabled = obj["yolo_draw_enabled"].toBool();
    if (yolo_draw_enabled_ != enabled) {
      yolo_draw_enabled_ = enabled;
      emit YoloDrawEnabledChanged();
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

void SystemDataModel::SetYoloDrawThreshold(double threshold) {
  if (threshold < 0.2) threshold = 0.2;
  if (threshold > 0.8) threshold = 0.8;
  if (!qFuzzyCompare(yolo_draw_threshold_ + 1.0, threshold + 1.0)) {
    yolo_draw_threshold_ = threshold;
    emit YoloDrawThresholdChanged();
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
  if (!recording && recording_elapsed_ != 0) {
    recording_elapsed_ = 0;
    emit RecordingElapsedChanged();
  }
  emit IsRecordingChanged();
  emit RecordingStatusChanged();
}

QString SystemDataModel::RecordingElapsedText() const {
  int sec = recording_elapsed_;
  if (sec < 0) sec = 0;
  int mm = sec / 60;
  int ss = sec % 60;
  return QString("%1:%2").arg(mm, 2, 10, QChar('0')).arg(ss, 2, 10, QChar('0'));
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
