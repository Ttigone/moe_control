#include <QFont>
#include <QGuiApplication>
#include <QLocale>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>
#include <QTranslator>

#include "logger.h"
#include "settings_manager.h"
#include "system_data_model.h"
#include "tcp_client.h"

#ifndef APP_BUILD_INFO
#define APP_BUILD_INFO "dev"
#endif

int main(int argc, char *argv[]) {
  Logger::Instance().Initialize();
#ifndef Q_OS_ANDROID
  qInstallMessageHandler(Logger::QtMessageHandler);
#endif

  // Qt虚拟键盘仅用于桌面端；Android有自己的系统IME，强制设置会覆盖原生输入法导致键盘异常
#ifndef Q_OS_ANDROID
  qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
#endif

  QGuiApplication app(argc, argv);

  app.setOrganizationName("MoeControl");
  app.setOrganizationDomain("moe.control");
  app.setApplicationName("MonitoringApp");
  app.setApplicationVersion(QStringLiteral(APP_BUILD_INFO));

  QFont appFont;
  appFont.setFamilies({"Microsoft YaHei UI", "Noto Sans SC", "Roboto",
                       "Noto Sans", "Segoe UI", "sans-serif"});
  app.setFont(appFont);

  SettingsManager settings_manager;

  QTranslator translator;
  // 根据设置加载对应语言，优先从QRC资源加载（Android必须走此路径）
  QString qmFile = "moe_control_" + settings_manager.language();
  QString qrcPath = ":/i18n/" + qmFile + ".qm";
  if (translator.load(qrcPath)) {
    app.installTranslator(&translator);
    LOG_INFO(QString("Translation file loaded from QRC: %1").arg(qrcPath));
  } else if (translator.load(qmFile, "./translations")) {
    app.installTranslator(&translator);
    LOG_INFO(QString("Translation file loaded from disk: %1").arg(qmFile));
  } else {
    LOG_WARN(QString("Failed to load translation file: %1").arg(qmFile));
  }

  // 监听语言切换信号
  QObject::connect(
      &settings_manager, &SettingsManager::LanguageChangeRequested,
      [&](const QString &lang) {
        LOG_INFO(QString("Language change requested: %1 (restart required)")
                     .arg(lang));
      });

  TcpClient tcp_client;
  SystemDataModel data_model;

  QObject::connect(&tcp_client, &TcpClient::DataReceived, &data_model,
                   &SystemDataModel::ParseServerData);

  QQmlApplicationEngine engine;

  qmlRegisterSingletonInstance("moe_control", 1, 0, "TcpClient", &tcp_client);
  qmlRegisterSingletonInstance("moe_control", 1, 0, "SystemDataModel",
                               &data_model);
  qmlRegisterSingletonInstance("moe_control", 1, 0, "SettingsManager",
                               &settings_manager);

  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() {
        LOG_ERROR("QML object creation failed");
        QCoreApplication::exit(-1);
      },
      Qt::QueuedConnection);
  engine.load(QUrl(QStringLiteral("qrc:/qt/qml/moe_control/Main.qml")));

  if (engine.rootObjects().isEmpty()) {
    LOG_ERROR("Failed to load QML root objects");
    return -1;
  }

  if (settings_manager.autoConnect()) {
    LOG_INFO("Auto-connecting to server...");
    tcp_client.ConnectToServer();
  }

  int result = app.exec();
  LOG_INFO("Application exiting normally");
  engine.clearComponentCache();

  return result;
}
