#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include <QObject>
#include <QSettings>
#include <QString>

class SettingsManager : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString language READ language WRITE SetLanguage NOTIFY LanguageChanged)
  Q_PROPERTY(bool autoConnect READ autoConnect WRITE SetAutoConnect NOTIFY AutoConnectChanged)
  Q_PROPERTY(bool keepScreenOn READ keepScreenOn WRITE SetKeepScreenOn NOTIFY KeepScreenOnChanged)

 public:
  explicit SettingsManager(QObject *parent = nullptr);
  ~SettingsManager();

  QString language() const { return language_; }
  bool autoConnect() const { return auto_connect_; }
  bool keepScreenOn() const { return keep_screen_on_; }

  Q_INVOKABLE void SetLanguage(const QString &lang);
  Q_INVOKABLE void SetAutoConnect(bool enable);
  Q_INVOKABLE void SetKeepScreenOn(bool enable);
  Q_INVOKABLE void SaveSettings();
  Q_INVOKABLE void LoadSettings();

 signals:
  void LanguageChanged();
  void AutoConnectChanged();
  void KeepScreenOnChanged();
  void LanguageChangeRequested(const QString &lang);

 private:
  QSettings *settings_;
  QString language_;
  bool auto_connect_;
  bool keep_screen_on_;
};

#endif // SETTINGS_MANAGER_H
