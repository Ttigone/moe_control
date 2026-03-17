#include "settings_manager.h"
#include "logger.h"

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent),
      settings_(new QSettings("MoeControl", "MonitoringApp", this)),
      language_("zh_CN"),
      auto_connect_(false),
      keep_screen_on_(false) {
  LoadSettings();
}

SettingsManager::~SettingsManager() {
  SaveSettings();
}

void SettingsManager::SetLanguage(const QString &lang) {
  if (language_ != lang) {
    language_ = lang;
    emit LanguageChanged();
    emit LanguageChangeRequested(lang);
    LOG_INFO(QString("Language changed to: %1").arg(lang));
  }
}

void SettingsManager::SetAutoConnect(bool enable) {
  if (auto_connect_ != enable) {
    auto_connect_ = enable;
    emit AutoConnectChanged();
    LOG_INFO(QString("Auto-connect: %1").arg(enable ? "enabled" : "disabled"));
  }
}

void SettingsManager::SetKeepScreenOn(bool enable) {
  if (keep_screen_on_ != enable) {
    keep_screen_on_ = enable;
    emit KeepScreenOnChanged();
    LOG_INFO(QString("Keep screen on: %1").arg(enable ? "enabled" : "disabled"));
  }
}

void SettingsManager::SaveSettings() {
  settings_->setValue("language", language_);
  settings_->setValue("autoConnect", auto_connect_);
  settings_->setValue("keepScreenOn", keep_screen_on_);
  settings_->sync();
  LOG_DEBUG("Settings saved");
}

void SettingsManager::LoadSettings() {
  language_ = settings_->value("language", "zh_CN").toString();
  auto_connect_ = settings_->value("autoConnect", false).toBool();
  keep_screen_on_ = settings_->value("keepScreenOn", false).toBool();
  LOG_DEBUG(QString("Settings loaded - Language: %1, AutoConnect: %2, KeepScreenOn: %3")
                .arg(language_)
                .arg(auto_connect_)
                .arg(keep_screen_on_));
}
