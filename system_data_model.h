#ifndef SYSTEMDATAMODEL_H
#define SYSTEMDATAMODEL_H

#include <QObject>
#include <QString>

class SystemDataModel : public QObject
{
    Q_OBJECT
    
    // 传感器数据
    Q_PROPERTY(double lightValue READ LightValue NOTIFY LightValueChanged)
    Q_PROPERTY(QString lightStatus READ LightStatus NOTIFY LightStatusChanged)
    
    // 门窗状态
    Q_PROPERTY(QString doorStatus READ DoorStatus NOTIFY DoorStatusChanged)
    Q_PROPERTY(bool doorOpen READ DoorOpen NOTIFY DoorOpenChanged)
    
    // 人体检测状态
    Q_PROPERTY(QString humanStatus READ HumanStatus NOTIFY HumanStatusChanged)
    Q_PROPERTY(int humanDetectLevel READ HumanDetectLevel NOTIFY HumanDetectLevelChanged) // 0:无人 1:疑似 2:确认
    
    // 报警状态
    Q_PROPERTY(QString alarmStatus READ AlarmStatus NOTIFY AlarmStatusChanged)
    Q_PROPERTY(bool alarmActive READ AlarmActive NOTIFY AlarmActiveChanged)
    
    // 录制状态
    Q_PROPERTY(QString recordingStatus READ RecordingStatus NOTIFY RecordingStatusChanged)
    Q_PROPERTY(bool isRecording READ IsRecording NOTIFY IsRecordingChanged)
    Q_PROPERTY(int recordingElapsed READ RecordingElapsed NOTIFY RecordingElapsedChanged)
    Q_PROPERTY(int recordingDuration READ RecordingDuration NOTIFY RecordingDurationChanged)
    Q_PROPERTY(QString recordingElapsedText READ RecordingElapsedText NOTIFY RecordingElapsedChanged)
    
    // 工作模式
    Q_PROPERTY(int workMode READ WorkMode NOTIFY WorkModeChanged) // 0:自动 1:手动 2:远程
    Q_PROPERTY(QString workModeText READ WorkModeText NOTIFY WorkModeTextChanged)
    
    // 阈值设置
    Q_PROPERTY(int alarmDelay READ AlarmDelay NOTIFY AlarmDelayChanged)
    Q_PROPERTY(double yoloThreshold READ YoloThreshold NOTIFY YoloThresholdChanged)
    Q_PROPERTY(double yoloDrawThreshold READ YoloDrawThreshold NOTIFY YoloDrawThresholdChanged)
    Q_PROPERTY(int inferInterval READ InferInterval NOTIFY InferIntervalChanged)
    Q_PROPERTY(double streamFps READ StreamFps NOTIFY StreamFpsChanged)
    Q_PROPERTY(double inferMs READ InferMs NOTIFY InferMsChanged)
    Q_PROPERTY(bool yoloEnabled READ YoloEnabled NOTIFY YoloEnabledChanged)
    Q_PROPERTY(bool yoloDrawEnabled READ YoloDrawEnabled NOTIFY YoloDrawEnabledChanged)

public:
    explicit SystemDataModel(QObject *parent = nullptr);

    // Getters
    double LightValue() const { return light_value_; }
    QString LightStatus() const { return light_status_; }
    
    QString DoorStatus() const { return door_status_; }
    bool DoorOpen() const { return door_open_; }
    
    QString HumanStatus() const { return human_status_; }
    int HumanDetectLevel() const { return human_detect_level_; }
    
    QString AlarmStatus() const { return alarm_status_; }
    bool AlarmActive() const { return alarm_active_; }
    
    QString RecordingStatus() const { return recording_status_; }
    bool IsRecording() const { return is_recording_; }
    int RecordingElapsed() const { return recording_elapsed_; }
    int RecordingDuration() const { return recording_duration_; }
    QString RecordingElapsedText() const;
    
    int WorkMode() const { return work_mode_; }
    QString WorkModeText() const { return work_mode_text_; }
    
    int AlarmDelay() const { return alarm_delay_; }
    double YoloThreshold() const { return yolo_threshold_; }
    double YoloDrawThreshold() const { return yolo_draw_threshold_; }
    int InferInterval() const { return infer_interval_; }
    double StreamFps() const { return stream_fps_; }
    double InferMs() const { return infer_ms_; }
    bool YoloEnabled() const { return yolo_enabled_; }
    bool YoloDrawEnabled() const { return yolo_draw_enabled_; }

    // 解析从服务器接收的数据
    Q_INVOKABLE void ParseServerData(const QString &json_data);
    
    // Setters（本地设置）
    Q_INVOKABLE void SetWorkMode(int mode);
    Q_INVOKABLE void SetAlarmDelay(int seconds);
    Q_INVOKABLE void SetYoloThreshold(double threshold);

signals:
    void LightValueChanged();
    void LightStatusChanged();
    void DoorStatusChanged();
    void DoorOpenChanged();
    void HumanStatusChanged();
    void HumanDetectLevelChanged();
    void AlarmStatusChanged();
    void AlarmActiveChanged();
    void RecordingStatusChanged();
    void IsRecordingChanged();
    void RecordingElapsedChanged();
    void RecordingDurationChanged();
    void WorkModeChanged();
    void WorkModeTextChanged();
    void AlarmDelayChanged();
    void YoloThresholdChanged();
    void YoloDrawThresholdChanged();
    void InferIntervalChanged();
    void StreamFpsChanged();
    void InferMsChanged();
    void YoloEnabledChanged();
    void YoloDrawEnabledChanged();

private:
    void UpdateLightStatus();
    void UpdateDoorStatus(bool open);
    void UpdateHumanStatus(int level);
    void UpdateAlarmStatus(bool active);
    void UpdateRecordingStatus(bool recording);
    void UpdateWorkModeText();

    // 传感器数据
    double light_value_;
    QString light_status_;
    
    // 门窗状态
    QString door_status_;
    bool door_open_;
    
    // 人体检测
    QString human_status_;
    int human_detect_level_;
    
    // 报警状态
    QString alarm_status_;
    bool alarm_active_;
    
    // 录制状态
    QString recording_status_;
    bool is_recording_;
    int recording_elapsed_;
    int recording_duration_;
    
    // 工作模式
    int work_mode_;
    QString work_mode_text_;
    
    // 阈值
    int alarm_delay_;
    double yolo_threshold_;
    double yolo_draw_threshold_;
    int infer_interval_;
    double stream_fps_;
    double infer_ms_;
    bool yolo_enabled_;
    bool yolo_draw_enabled_;
};

#endif // SYSTEMDATAMODEL_H
