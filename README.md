# 智能监控系统 Android APP

## 项目简介

这是一个基于 Qt 6.8.3 开发的 Android 视频监控系统客户端，配合 RK3576 开发板使用，实现远程监控、传感器数据查看、智能报警控制等功能。

## 新功能亮点 🌟

### 国际化支持
- ✅ 使用 Qt Linguist 翻译系统
- ✅ 支持中文/英文界面切换
- ✅ 避免硬编码中文导致的乱码问题
- ✅ 翻译文件：`translations/moe_control_zh_CN.ts`

### 专业日志系统
- ✅ **spdlog 1.12.0** 高性能日志库
- ✅ **Android logcat 集成**：通过 `adb logcat` 查看日志
- ✅ **多 Sink 输出**：控制台、文件、Android 日志
- ✅ **Qt 日志桥接**：自动捕获所有 qDebug/qInfo/qWarning
- ✅ **日志等级区分**：qDebug→TRACE, qInfo→INFO, qWarning→WARN

### Google C++ 代码规范
- ✅ 成员变量：`snake_case_` 小写下划线分隔，尾部下划线
- ✅ 函数/方法：`PascalCase` 首字母大写驼峰
- ✅ 提高代码可读性和维护性

## 功能特性

### 1. 数据展示区
- **光照强度**: 实时显示BH1750传感器采集的光照值（lux）
- **门窗状态**: 显示磁簧开关检测的门窗开合状态
- **人体检测**: 显示三级检测状态（无人/疑似有人/确认有人）
- **报警状态**: 显示当前是否处于报警状态
- **录制状态**: 显示视频录制状态

### 2. 工作模式控制
- **自动模式**: 系统自动运行双重人体检测和报警联动
- **手动模式**: 支持手动触发/停止报警、开始/停止录制
- **远程模式**: 远程查看数据和调节参数

### 3. 阈值调节
- **安防触发延迟**: 可调节5-30秒，控制报警录像时长
- **YOLO置信度**: 可调节0.4-0.8，控制人体识别灵敏度

### 4. 视频监控
- 一键跳转浏览器查看Web实时监控画面

## 技术架构

### 通信协议
- 使用TCP协议与RK3576板子通信
- 数据格式：JSON
- 默认端口：8888

### JSON协议格式

#### 客户端发送指令
```json
// 切换工作模式
{"cmd": "switch_mode", "mode": 0}  // 0:自动 1:手动 2:远程

// 设置报警延迟
{"cmd": "set_alarm_delay", "value": 10}  // 5-30秒

// 设置YOLO阈值
{"cmd": "set_yolo_threshold", "value": 0.6}  // 0.4-0.8

// 触发报警
{"cmd": "trigger_alarm"}

// 停止报警
{"cmd": "stop_alarm"}

// 开始录制
{"cmd": "start_recording", "duration": 10}  // 录制时长（秒）

// 停止录制
{"cmd": "stop_recording"}
```

#### 服务器返回数据
```json
{
  "light": 350.5,        // 光照值（lux）
  "door": false,         // 门窗状态（true:开启 false:关闭）
  "human": 0,           // 人体检测（0:无人 1:疑似 2:确认）
  "alarm": false,       // 报警状态（true:报警中 false:正常）
  "recording": false,   // 录制状态（true:录制中 false:未录制）
  "mode": 0,           // 当前工作模式（0:自动 1:手动 2:远程）
  "alarm_delay": 10,   // 当前报警延迟设置
  "yolo_threshold": 0.6 // 当前YOLO阈值设置
}
```

## 使用说明

### 1. 连接设置
1. 打开APP后，在顶部输入RK3576的IP地址和端口
2. 默认IP：192.168.1.100，端口：8888
3. 点击"连接"按钮建立TCP连接
4. 连接成功后，状态指示灯变为绿色

### 2. 模式切换
- 点击"自动"、"手动"、"远程"按钮切换工作模式
- 当前模式会高亮显示
- 手动模式下会显示额外的控制按钮

### 3. 手动控制（手动模式下）
- **触发报警**: 手动启动声光报警
- **停止报警**: 停止当前报警
- **开始录制**: 手动开始视频录制，可设置录制时长（10-60秒）
- **停止录制**: 停止当前录制

### 4. 阈值调节
- 拖动滑块调节参数
- 调节后自动同步到服务器
- **安防触发延迟**: 控制报警持续时间和录像时长
- **YOLO置信度**: 数值越高识别越严格，越低越灵敏

### 5. 查看监控
- 点击"查看实时监控"按钮
- 自动在浏览器中打开Web监控页面
- 默认访问地址：http://服务器IP:80

## 项目结构

```
moe_control/
├── main.cpp                    # 程序入口
├── Main.qml                    # 主界面UI
├── tcpclient.h/cpp            # TCP通信管理类
├── systemdatamodel.h/cpp      # 数据模型类
├── AndroidManifest.xml        # Android权限配置
└── CMakeLists.txt             # CMake构建配置
```

## 核心类说明

### TcpClient
TCP通信管理类，负责：
- 与服务器建立/断开连接
- 发送控制指令（JSON格式）
- 接收服务器数据
- 自动重连机制

### SystemDataModel
数据模型类，负责：
- 存储传感器数据和系统状态
- 解析服务器返回的JSON数据
- 提供QML数据绑定
- 状态变化通知

## 翻译更新流程 🌍

### 1. 更新源码中的翻译字符串
```cpp
// C++ 中使用 tr()
emit ConnectionStatusChanged(tr("Connected"));

// QML 中使用 qsTr()
Button { text: qsTr("Connect") }
```

### 2. 提取翻译字符串
```bash
lupdate . -ts translations/moe_control_zh_CN.ts
```

### 3. 编辑翻译文件
使用 **Qt Linguist** 工具打开 `translations/moe_control_zh_CN.ts`：
```bash
linguist translations/moe_control_zh_CN.ts
```
填写 `<translation>` 标签中的中文翻译。

### 4. 编译翻译文件
CMake 构建时会自动调用 `qt_add_translations` 生成 `.qm` 文件。

## 日志使用指南 📝

### C++ 日志宏
```cpp
#include "logger.h"

LOG_TRACE("Detailed debug info");
LOG_DEBUG("Development debug message");
LOG_INFO(QString("Connected to %1:%2").arg(address).arg(port));
LOG_WARN("Reconnecting to server...");
LOG_ERROR(QString("Socket error: %1").arg(errorString));
LOG_CRITICAL("Fatal error occurred!");
```

### Qt 日志自动捕获
所有 Qt 日志会自动桥接到 spdlog：
```cpp
qDebug() << "This goes to spdlog TRACE";
qInfo() << "This goes to spdlog INFO";
qWarning() << "This goes to spdlog WARN";
qCritical() << "This goes to spdlog ERROR";
```

### Android 日志查看
```bash
# 查看所有日志
adb logcat -s moe_control:*

# 仅查看 INFO 及以上级别
adb logcat -s moe_control:I

# 实时查看带颜色输出（配合 grep）
adb logcat -v color | grep moe_control
```

### 日志文件位置
- **Android**: `/sdcard/Android/data/<package>/files/logs/moe_control.log`
- **桌面**: `<可执行文件目录>/logs/moe_control.log`
- **轮转策略**: 单个文件最大 10MB，保留 3 个历史文件

## 编译要求

- Qt 6.8.3 或更高版本
- Qt模块：Quick, Network, VirtualKeyboard, **LinguistTools**
- Android NDK 25.1.8937393
- CMake 3.16+
- 目标平台：Android 6.0 (API 23) 或更高
- **spdlog 1.12.0** (通过 CMake FetchContent 自动下载)

## 服务器端要求

RK3576板子需要实现：
1. TCP服务器（端口8888）
2. JSON格式数据通信
3. Boa Web服务器（端口80）
4. MJPG-Streamer视频流服务

## 状态颜色说明

- **绿色**: 正常状态
- **橙色**: 警告状态（门窗开启、疑似有人）
- **红色**: 异常状态（确认有人、报警中、录制中）
- **蓝色/主题色**: 常规信息

## 注意事项

1. **网络连接**: 确保手机与RK3576在同一局域网内
2. **权限申请**: 首次运行需授予网络访问权限
3. **自动重连**: 连接断开后会每5秒自动尝试重连
4. **参数范围**: 请在规定范围内调节阈值，超出范围将被忽略
5. **视频查看**: 需要RK3576的Web服务正常运行

## 故障排查

### 无法连接服务器
- 检查IP地址和端口是否正确
- 确认手机和板子在同一网络
- 检查防火墙设置
- 查看服务器端TCP服务是否启动

### 数据不更新
- 检查TCP连接状态（右上角指示灯）
- 尝试断开重连
- 检查服务器端数据推送

### 无法查看监控
- 确认服务器IP地址正确
- 检查Boa Web服务是否运行
- 尝试直接在浏览器输入地址访问

## 开发者信息

这是一个毕业设计项目，基于RK3576平台的视频监控系统的Android客户端实现。

## 许可证

本项目仅用于学习和研究目的。


# 服务端

你需要在RK3576板子端实现：

TCP服务器（端口8888）
传感器数据采集和JSON推送
接收并处理APP指令
Boa Web服务器配置
MJPG-Streamer视频流服务

yolo 模型识别