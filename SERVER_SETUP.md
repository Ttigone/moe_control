# 快速配置指南

## 服务器端配置要求

为了让APP正常工作，RK3576服务器端需要实现以下功能：

### 1. TCP服务器

创建一个TCP服务器监听端口8888，接收APP发送的控制指令并返回传感器数据。

#### 数据推送频率
建议每1-2秒向所有连接的客户端推送一次完整数据：

```json
{
  "light": 350.5,
  "door": false,
  "human": 0,
  "alarm": false,
  "recording": false,
  "mode": 0,
  "alarm_delay": 10,
  "yolo_threshold": 0.6
}
```

#### 指令处理
接收并处理以下JSON格式指令：

| 指令 | 格式 | 说明 |
|------|------|------|
| 切换模式 | `{"cmd":"switch_mode","mode":0}` | mode: 0=自动,1=手动,2=远程 |
| 设置延迟 | `{"cmd":"set_alarm_delay","value":10}` | value: 5-30秒 |
| 设置阈值 | `{"cmd":"set_yolo_threshold","value":0.6}` | value: 0.4-0.8 |
| 触发报警 | `{"cmd":"trigger_alarm"}` | 手动触发报警 |
| 停止报警 | `{"cmd":"stop_alarm"}` | 停止当前报警 |
| 开始录制 | `{"cmd":"start_recording","duration":10}` | duration: 录制时长 |
| 停止录制 | `{"cmd":"stop_recording"}` | 停止当前录制 |

### 2. Web服务器配置

#### Boa配置
- 监听端口：80
- 网页根目录：包含视频流展示页面
- 支持静态文件访问

#### MJPG-Streamer配置
```bash
# 启动MJPG-Streamer示例
mjpg_streamer -i "input_uvc.so -d /dev/video0 -r 1920x1080 -f 30" \
              -o "output_http.so -p 8080 -w /usr/local/share/mjpg-streamer/www"
```

- 视频流端口：8080
- 视频流地址：`http://<IP>:8080/?action=stream`

#### 网页示例
在Boa的网页目录创建index.html：

```html
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>智能监控</title>
    <style>
        body {
            margin: 0;
            padding: 0;
            background: #000;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
        }
        img {
            max-width: 100%;
            max-height: 100vh;
            object-fit: contain;
        }
    </style>
</head>
<body>
    <img src="http://localhost:8080/?action=stream" alt="实时监控">
</body>
</html>
```

### 3. Python服务器示例代码

```python
#!/usr/bin/env python3
import socket
import json
import threading
import time

class MonitorServer:
    def __init__(self, host='0.0.0.0', port=8888):
        self.host = host
        self.port = port
        self.clients = []
        self.running = True
        
        # 系统状态
        self.status = {
            "light": 0.0,
            "door": False,
            "human": 0,
            "alarm": False,
            "recording": False,
            "mode": 0,
            "alarm_delay": 10,
            "yolo_threshold": 0.6
        }
    
    def start(self):
        """启动TCP服务器"""
        server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server.bind((self.host, self.port))
        server.listen(5)
        print(f"服务器启动在 {self.host}:{self.port}")
        
        # 启动数据推送线程
        push_thread = threading.Thread(target=self.push_data_loop)
        push_thread.daemon = True
        push_thread.start()
        
        while self.running:
            try:
                client_socket, addr = server.accept()
                print(f"客户端连接: {addr}")
                
                client_thread = threading.Thread(
                    target=self.handle_client,
                    args=(client_socket, addr)
                )
                client_thread.daemon = True
                client_thread.start()
            except Exception as e:
                print(f"接受连接错误: {e}")
    
    def handle_client(self, client_socket, addr):
        """处理客户端连接"""
        self.clients.append(client_socket)
        
        try:
            while self.running:
                data = client_socket.recv(1024).decode('utf-8')
                if not data:
                    break
                
                # 处理接收到的指令
                self.process_command(data)
                
        except Exception as e:
            print(f"处理客户端 {addr} 错误: {e}")
        finally:
            if client_socket in self.clients:
                self.clients.remove(client_socket)
            client_socket.close()
            print(f"客户端断开: {addr}")
    
    def process_command(self, data):
        """处理客户端指令"""
        try:
            cmd = json.loads(data)
            cmd_type = cmd.get("cmd")
            
            if cmd_type == "switch_mode":
                self.status["mode"] = cmd.get("mode", 0)
                print(f"切换模式: {self.status['mode']}")
                
            elif cmd_type == "set_alarm_delay":
                value = cmd.get("value", 10)
                if 5 <= value <= 30:
                    self.status["alarm_delay"] = value
                    print(f"设置报警延迟: {value}秒")
                    
            elif cmd_type == "set_yolo_threshold":
                value = cmd.get("value", 0.6)
                if 0.4 <= value <= 0.8:
                    self.status["yolo_threshold"] = value
                    print(f"设置YOLO阈值: {value}")
                    
            elif cmd_type == "trigger_alarm":
                self.status["alarm"] = True
                print("触发报警")
                # 这里添加实际的报警逻辑
                
            elif cmd_type == "stop_alarm":
                self.status["alarm"] = False
                print("停止报警")
                # 这里添加停止报警的逻辑
                
            elif cmd_type == "start_recording":
                duration = cmd.get("duration", 10)
                self.status["recording"] = True
                print(f"开始录制 {duration}秒")
                # 这里添加录制逻辑
                
            elif cmd_type == "stop_recording":
                self.status["recording"] = False
                print("停止录制")
                # 这里添加停止录制的逻辑
                
        except json.JSONDecodeError:
            print(f"JSON解析错误: {data}")
        except Exception as e:
            print(f"处理指令错误: {e}")
    
    def push_data_loop(self):
        """定时推送数据到所有客户端"""
        while self.running:
            # 这里应该读取实际的传感器数据
            # 示例：模拟更新光照值
            import random
            self.status["light"] = random.uniform(100, 500)
            
            # 推送数据
            self.broadcast_status()
            time.sleep(1)  # 每秒推送一次
    
    def broadcast_status(self):
        """向所有客户端广播状态"""
        data = json.dumps(self.status) + "\n"
        disconnected = []
        
        for client in self.clients:
            try:
                client.send(data.encode('utf-8'))
            except:
                disconnected.append(client)
        
        # 移除断开的客户端
        for client in disconnected:
            if client in self.clients:
                self.clients.remove(client)

if __name__ == "__main__":
    server = MonitorServer()
    server.start()
```

### 4. 运行服务

#### 启动TCP服务器
```bash
python3 monitor_server.py
```

#### 启动Web服务
```bash
# 启动Boa
boa -c /etc/boa

# 启动MJPG-Streamer
mjpg_streamer -i "input_uvc.so -d /dev/video0" -o "output_http.so -p 8080"
```

### 5. 测试连接

#### 测试TCP连接
```bash
# 使用telnet测试
telnet <RK3576_IP> 8888

# 发送测试指令
{"cmd":"switch_mode","mode":1}
```

#### 测试Web访问
在浏览器打开：`http://<RK3576_IP>:80`

## APP配置

1. 打开APP
2. 输入RK3576的IP地址（如：192.168.1.100）
3. 输入端口号：8888
4. 点击"连接"
5. 等待连接成功（右上角变绿色）

## 常见问题

### Q: APP无法连接服务器
A: 检查：
- RK3576和手机是否在同一网络
- 防火墙是否允许8888端口
- TCP服务是否正常运行

### Q: 数据不更新
A: 确保服务器端定时推送数据（建议1-2秒一次）

### Q: 无法查看视频
A: 检查：
- Boa服务是否运行
- MJPG-Streamer是否运行
- 浏览器是否支持MJPEG格式

## 下一步

1. 根据实际硬件连接传感器数据采集
2. 实现YOLO人体检测逻辑
3. 实现报警和录制功能
4. 优化视频流性能
