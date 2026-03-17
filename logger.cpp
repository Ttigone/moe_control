#include "logger.h"

#ifndef Q_OS_ANDROID
// 桌面平台使用spdlog
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#endif

#include <QDateTime>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

Logger &Logger::Instance() {
  static Logger instance;
  return instance;
}

Logger::Logger() : QObject(nullptr), initialized_(false) {}

Logger::~Logger() {
#ifndef Q_OS_ANDROID
  if (file_logger_) {
    file_logger_->flush();
  }
  if (console_logger_) {
    console_logger_->flush();
  }
  spdlog::shutdown();
#endif
}

void Logger::Initialize(const QString &log_file_path) {
  if (initialized_) {
    return;
  }

#ifdef Q_OS_ANDROID
  // Android平台：简化为只使用Qt日志系统
  initialized_ = true;
  qInfo() << "Logger initialized (Android mode - using Qt logging)";
  return;
#else
  try {
    // 设置日志格式
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v");

    // 桌面平台：使用控制台
    InitializeConsoleLogger();

    // 文件日志
    if (log_file_path.isEmpty()) {
      QString default_path = QStandardPaths::writableLocation(
          QStandardPaths::AppLocalDataLocation);
      QDir dir(default_path);
      if (!dir.exists()) {
        dir.mkpath(".");
      }

      QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd");
      QString file_path =
          dir.filePath(QString("moe_control_%1.log").arg(timestamp));
      InitializeFileLogger(file_path);
    } else {
      InitializeFileLogger(log_file_path);
    }

    // 设置默认日志级别
    SetLevel(Level::Debug);

    initialized_ = true;

    Info("Logger initialized successfully");

  } catch (const spdlog::spdlog_ex &ex) {
    qCritical() << "Log initialization failed:" << ex.what();
  }
#endif
}

#ifndef Q_OS_ANDROID
void Logger::InitializeConsoleLogger() {
  try {
    console_logger_ = spdlog::stdout_color_mt("console");
    console_logger_->set_level(spdlog::level::trace);
  } catch (const spdlog::spdlog_ex &ex) {
    qCritical() << "Console logger creation failed:" << ex.what();
  }
}

void Logger::InitializeFileLogger(const QString &file_path) {
  try {
    // 使用循环文件日志，最大5MB，保留3个文件
    file_logger_ = spdlog::rotating_logger_mt("file", file_path.toStdString(),
                                              1024 * 1024 * 5,  // 5MB
                                              3                 // 3个文件
    );
    file_logger_->set_level(spdlog::level::trace);
    file_logger_->flush_on(spdlog::level::warn);  // warn级别及以上立即刷新
  } catch (const spdlog::spdlog_ex &ex) {
    qCritical() << "File logger creation failed:" << ex.what();
  }
}

void Logger::SetLevel(Level level) {
  spdlog::level::level_enum spdlog_level;

  switch (level) {
    case Level::Trace:
      spdlog_level = spdlog::level::trace;
      break;
    case Level::Debug:
      spdlog_level = spdlog::level::debug;
      break;
    case Level::Info:
      spdlog_level = spdlog::level::info;
      break;
    case Level::Warning:
      spdlog_level = spdlog::level::warn;
      break;
    case Level::Error:
      spdlog_level = spdlog::level::err;
      break;
    case Level::Critical:
      spdlog_level = spdlog::level::critical;
      break;
  }

  spdlog::set_level(spdlog_level);

  if (console_logger_) {
    console_logger_->set_level(spdlog_level);
  }
  if (file_logger_) {
    file_logger_->set_level(spdlog_level);
  }
}
#else
// Android平台的空实现
void Logger::InitializeConsoleLogger() {}
void Logger::InitializeFileLogger(const QString &) {}
void Logger::SetLevel(Level) {}
#endif

// 日志输出函数
void Logger::Trace(const QString &message) {
#ifdef Q_OS_ANDROID
  qDebug() << message;
#else
  std::string msg = message.toStdString();
  if (console_logger_) console_logger_->trace(msg);
  if (file_logger_) file_logger_->trace(msg);
#endif
}

void Logger::Debug(const QString &message) {
#ifdef Q_OS_ANDROID
  qDebug() << message;
#else
  std::string msg = message.toStdString();
  if (console_logger_) console_logger_->debug(msg);
  if (file_logger_) file_logger_->debug(msg);
#endif
}

void Logger::Info(const QString &message) {
#ifdef Q_OS_ANDROID
  qInfo() << message;
#else
  std::string msg = message.toStdString();
  if (console_logger_) console_logger_->info(msg);
  if (file_logger_) file_logger_->info(msg);
#endif
}

void Logger::Warning(const QString &message) {
#ifdef Q_OS_ANDROID
  qWarning() << message;
#else
  std::string msg = message.toStdString();
  if (console_logger_) console_logger_->warn(msg);
  if (file_logger_) file_logger_->warn(msg);
#endif
}

void Logger::Error(const QString &message) {
#ifdef Q_OS_ANDROID
  qCritical() << message;
#else
  std::string msg = message.toStdString();
  if (console_logger_) console_logger_->error(msg);
  if (file_logger_) file_logger_->error(msg);
#endif
}

void Logger::Critical(const QString &message) {
#ifdef Q_OS_ANDROID
  qCritical() << message;
#else
  std::string msg = message.toStdString();
  if (console_logger_) console_logger_->critical(msg);
  if (file_logger_) file_logger_->critical(msg);
#endif
}

void Logger::QtMessageHandler(QtMsgType type, const QMessageLogContext &context,
                              const QString &msg) {
  Q_UNUSED(context)
  
  switch (type) {
    case QtDebugMsg:
      Instance().Debug(msg);
      break;
    case QtInfoMsg:
      Instance().Info(msg);
      break;
    case QtWarningMsg:
      Instance().Warning(msg);
      break;
    case QtCriticalMsg:
      Instance().Error(msg);
      break;
    case QtFatalMsg:
      Instance().Critical(msg);
      abort();
  }
}
