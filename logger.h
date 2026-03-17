#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>
#include <memory>

// 前置声明spdlog类型
namespace spdlog {
    class logger;
}

/**
 * @brief 日志管理类
 * 
 * 封装spdlog库，提供统一的日志接口，支持：
 * - 多个日志级别（trace, debug, info, warn, error, critical）
 * - 文件日志输出
 * - 控制台日志输出
 * - Qt日志系统桥接
 * - Android logcat支持
 */
class Logger : public QObject
{
    Q_OBJECT
    
public:
    enum class Level {
        Trace,
        Debug,
        Info,
        Warning,
        Error,
        Critical
    };
    
    static Logger& Instance();
    
    // 初始化日志系统
    void Initialize(const QString &log_file_path = QString());
    
    // 设置日志级别
    void SetLevel(Level level);
    
    // 日志输出接口
    void Trace(const QString &message);
    void Debug(const QString &message);
    void Info(const QString &message);
    void Warning(const QString &message);
    void Error(const QString &message);
    void Critical(const QString &message);
    
    // Qt消息处理器（用于捕获qDebug等）
    static void QtMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    
private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    void InitializeConsoleLogger();
    void InitializeFileLogger(const QString &file_path);
    
#ifndef Q_OS_ANDROID
    std::shared_ptr<spdlog::logger> console_logger_;
    std::shared_ptr<spdlog::logger> file_logger_;
#endif
    bool initialized_;
};

// 便捷宏定义
#define LOG_TRACE(msg) Logger::Instance().Trace(msg)
#define LOG_DEBUG(msg) Logger::Instance().Debug(msg)
#define LOG_INFO(msg) Logger::Instance().Info(msg)
#define LOG_WARN(msg) Logger::Instance().Warning(msg)
#define LOG_ERROR(msg) Logger::Instance().Error(msg)
#define LOG_CRITICAL(msg) Logger::Instance().Critical(msg)

#endif // LOGGER_H
