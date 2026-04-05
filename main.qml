import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.VirtualKeyboard
import moe_control

ApplicationWindow {
    id: window
    width: 360
    height: 640
    visible: true
    title: qsTr("Intelligent Monitoring System")

    property int androidTopInset: Qt.platform.os === "android" ? 24 : 0

    property TcpClient tcpClient: TcpClient
    property SystemDataModel dataModel: SystemDataModel
    property SettingsManager settingsManager: SettingsManager

    Component.onCompleted: {
        tcpClient.DataReceived.connect(function (data) {
            dataModel.ParseServerData(data)
        })
    }
    
    // 设置对话框
    SettingsDialog {
        id: settingsDialog
        parent: Overlay.overlay
        settingsManager: window.settingsManager
    }

    header: ToolBar {
        implicitHeight: 56 + window.androidTopInset

        RowLayout {
            anchors.fill: parent
            anchors.topMargin: window.androidTopInset
            spacing: 8
            
            Label {
                text: qsTr("Monitoring")
                font.pixelSize: 18
                font.bold: true
                Layout.fillWidth: true
                leftPadding: 12
                elide: Text.ElideRight
            }

            Rectangle {
                width: 8
                height: 8
                radius: 4
                color: tcpClient.connected ? "#4CAF50" : "#F44336"
            }

            Label {
                text: tcpClient.connected ? qsTr("Connected") : qsTr(
                                                "Disconnected")
                font.pixelSize: 11
                Layout.rightMargin: 4
            }
            
            ToolButton {
                icon.name: "settings"
                text: "⚙"
                font.pixelSize: 18
                onClicked: settingsDialog.open()
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Settings")
                Layout.rightMargin: 4
            }
        }
    }

    ScrollView {
        anchors.fill: parent
        contentWidth: availableWidth

        ColumnLayout {
            width: parent.width
            spacing: 0

            // 连接设置区域
            Pane {
                Layout.fillWidth: true
                Material.elevation: 2

                ColumnLayout {
                    width: parent.width
                    spacing: 12

                    Label {
                        text: qsTr("Connection Settings")
                        font.pixelSize: 16
                        font.bold: true
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        TextField {
                            id: serverIpField
                            Layout.fillWidth: true
                            placeholderText: qsTr("Server IP")
                            text: tcpClient.serverAddress
                            inputMethodHints: Qt.ImhDigitsOnly
                        }

                        TextField {
                            id: serverPortField
                            Layout.preferredWidth: 80
                            placeholderText: qsTr("Port")
                            text: tcpClient.serverPort.toString()
                            inputMethodHints: Qt.ImhDigitsOnly
                        }

                        Button {
                            text: tcpClient.connected ? qsTr("Disconnect") : qsTr(
                                                            "Connect")
                            highlighted: true
                            onClicked: {
                                if (tcpClient.connected) {
                                    tcpClient.DisconnectFromServer()
                                } else {
                                    tcpClient.serverAddress = serverIpField.text
                                    tcpClient.serverPort = parseInt(
                                                serverPortField.text)
                                    tcpClient.ConnectToServer()
                                }
                            }
                        }
                    }

                    Label {
                        text: qsTr("UI Build: ") + Qt.application.version
                        font.pixelSize: 11
                        color: Material.hintTextColor
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Label {
                            text: qsTr("YOLO Detection:")
                            font.pixelSize: 13
                        }
                        Label {
                            text: dataModel.yoloEnabled ? qsTr("Enabled") : qsTr("Disabled")
                            font.pixelSize: 13
                            font.bold: true
                            color: dataModel.yoloEnabled ? "#4CAF50" : "#EF5350"
                            Layout.fillWidth: true
                            horizontalAlignment: Text.AlignRight
                        }
                        Switch {
                            checked: dataModel.yoloEnabled
                            enabled: dataModel.workMode !== 0
                            onToggled: tcpClient.SetYoloEnabled(checked)
                        }
                    }
                }
            }

            // 数据展示区
            Pane {
                Layout.fillWidth: true
                Layout.topMargin: 8
                Material.elevation: 2

                ColumnLayout {
                    width: parent.width
                    spacing: 16

                    Label {
                        text: qsTr("Sensor Data")
                        font.pixelSize: 16
                        font.bold: true
                    }

                    GridLayout {
                        Layout.fillWidth: true
                        columns: 2
                        columnSpacing: 16
                        rowSpacing: 12

                        // 光照值
                        Label {
                            text: qsTr("Light Intensity:")
                            font.pixelSize: 14
                        }
                        Label {
                            text: dataModel.lightValue.toFixed(
                                      1) + " lux (" + dataModel.lightStatus + ")"
                            font.pixelSize: 14
                            font.bold: true
                            color: Material.accent
                        }

                        // 门窗状态
                        Label {
                            text: qsTr("Door Status:")
                            font.pixelSize: 14
                        }
                        Label {
                            text: dataModel.doorStatus
                            font.pixelSize: 14
                            font.bold: true
                            color: dataModel.doorOpen ? "#FF9800" : Material.accent
                        }

                        // 人体检测
                        Label {
                            text: qsTr("Human Detection:")
                            font.pixelSize: 14
                        }
                        Label {
                            text: dataModel.humanStatus
                            font.pixelSize: 14
                            font.bold: true
                            color: dataModel.humanDetectLevel
                                   === 2 ? "#F44336" : dataModel.humanDetectLevel
                                           === 1 ? "#FF9800" : Material.accent
                        }

                        // 报警状态
                        Label {
                            text: qsTr("Alarm Status:")
                            font.pixelSize: 14
                        }
                        Label {
                            text: dataModel.alarmStatus
                            font.pixelSize: 14
                            font.bold: true
                            color: dataModel.alarmActive ? "#F44336" : Material.accent
                        }

                        // 录制状态
                        Label {
                            text: qsTr("Recording Status:")
                            font.pixelSize: 14
                        }
                        Label {
                            text: dataModel.recordingStatus
                            font.pixelSize: 14
                            font.bold: true
                            color: dataModel.isRecording ? "#F44336" : Material.accent
                        }
                    }
                }
            }

            // 模式控制区
            Pane {
                Layout.fillWidth: true
                Layout.topMargin: 8
                Material.elevation: 2

                ColumnLayout {
                    width: parent.width
                    spacing: 16

                    Label {
                        text: qsTr("Work Mode")
                        font.pixelSize: 16
                        font.bold: true
                    }

                    Label {
                        text: qsTr("Current Mode: ") + dataModel.workModeText
                        font.pixelSize: 14
                        color: Material.accent
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        Button {
                            text: qsTr("Auto")
                            Layout.fillWidth: true
                            highlighted: dataModel.workMode === 0
                            onClicked: {
                                tcpClient.SwitchMode(0)
                                dataModel.SetWorkMode(0)
                            }
                        }

                        Button {
                            text: qsTr("Manual")
                            Layout.fillWidth: true
                            highlighted: dataModel.workMode === 1
                            onClicked: {
                                tcpClient.SwitchMode(1)
                                dataModel.SetWorkMode(1)
                            }
                        }

                        Button {
                            text: qsTr("Remote")
                            Layout.fillWidth: true
                            highlighted: dataModel.workMode === 2
                            onClicked: {
                                tcpClient.SwitchMode(2)
                                dataModel.SetWorkMode(2)
                            }
                        }
                    }

                    // 紧急停止：报警中时任意模式可执行
                    Button {
                        visible: dataModel.alarmActive
                        text: qsTr("Emergency Stop Alarm")
                        Layout.fillWidth: true
                        Material.background: "#F44336"
                        Material.foreground: "white"
                        onClicked: tcpClient.StopAlarm()
                    }

                    // 手动模式控制按钮
                    ColumnLayout {
                        Layout.fillWidth: true
                        visible: dataModel.workMode === 1
                        spacing: 8

                        Label {
                            text: qsTr("Manual Control")
                            font.pixelSize: 14
                            font.bold: true
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 8

                            Button {
                                text: qsTr("Trigger Alarm")
                                Layout.fillWidth: true
                                Material.background: "#F44336"
                                Material.foreground: "white"
                                onClicked: tcpClient.TriggerAlarm()
                            }

                            Button {
                                text: qsTr("Stop Alarm")
                                Layout.fillWidth: true
                                onClicked: tcpClient.StopAlarm()
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 8

                            Button {
                                text: dataModel.isRecording ? qsTr("Stop Recording") : qsTr(
                                                                  "Start Recording")
                                Layout.fillWidth: true
                                highlighted: dataModel.isRecording
                                onClicked: {
                                    if (dataModel.isRecording) {
                                        tcpClient.StopRecording()
                                    } else {
                                        tcpClient.StartRecording(
                                                    parseInt(
                                                        recordDurationField.text))
                                    }
                                }
                            }

                            TextField {
                                id: recordDurationField
                                Layout.preferredWidth: 60
                                text: "10"
                                placeholderText: qsTr("seconds")
                                inputMethodHints: Qt.ImhDigitsOnly
                                validator: IntValidator {
                                    bottom: 10
                                    top: 60
                                }
                            }

                            Label {
                                text: qsTr("seconds")
                                font.pixelSize: 14
                            }
                        }
                    }
                }
            }

            // 阈值调节区
            Pane {
                Layout.fillWidth: true
                Layout.topMargin: 8
                Material.elevation: 2

                ColumnLayout {
                    width: parent.width
                    spacing: 16

                    Label {
                        text: qsTr("Threshold Settings")
                        font.pixelSize: 16
                        font.bold: true
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Label {
                            text: qsTr("YOLO Detection:")
                            font.pixelSize: 14
                        }
                        Label {
                            text: dataModel.yoloEnabled ? qsTr("Enabled") : qsTr("Disabled")
                            font.pixelSize: 14
                            font.bold: true
                            color: dataModel.yoloEnabled ? "#4CAF50" : "#EF5350"
                            Layout.fillWidth: true
                            horizontalAlignment: Text.AlignRight
                        }
                        Switch {
                            checked: dataModel.yoloEnabled
                            enabled: dataModel.workMode !== 0
                            onToggled: tcpClient.SetYoloEnabled(checked)
                        }
                    }

                    Label {
                        visible: dataModel.workMode === 0
                        text: qsTr("Auto mode uses PIR+Door to trigger YOLO automatically")
                        font.pixelSize: 11
                        color: Material.hintTextColor
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                    }

                    // 安防触发延迟
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        RowLayout {
                            Layout.fillWidth: true
                            Label {
                                text: qsTr("Alarm Trigger Delay:")
                                font.pixelSize: 14
                            }
                            Label {
                                text: dataModel.alarmDelay + qsTr(" seconds")
                                font.pixelSize: 14
                                font.bold: true
                                color: Material.accent
                                Layout.fillWidth: true
                                horizontalAlignment: Text.AlignRight
                            }
                        }

                        Slider {
                            id: delaySlider
                            Layout.fillWidth: true
                            from: 5
                            to: 30
                            stepSize: 1
                            value: dataModel.alarmDelay
                            onMoved: {
                                dataModel.SetAlarmDelay(Math.round(value))
                                tcpClient.SetAlarmDelay(Math.round(value))
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            Label {
                                text: qsTr("5s")
                                font.pixelSize: 12
                                color: Material.hintTextColor
                            }
                            Item {
                                Layout.fillWidth: true
                            }
                            Label {
                                text: qsTr("30s")
                                font.pixelSize: 12
                                color: Material.hintTextColor
                            }
                        }
                    }

                    // YOLO置信度阈值
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        RowLayout {
                            Layout.fillWidth: true
                            Label {
                                text: qsTr("YOLO Confidence:")
                                font.pixelSize: 14
                            }
                            Label {
                                text: dataModel.yoloThreshold.toFixed(2)
                                font.pixelSize: 14
                                font.bold: true
                                color: Material.accent
                                Layout.fillWidth: true
                                horizontalAlignment: Text.AlignRight
                            }
                        }

                        Slider {
                            id: thresholdSlider
                            Layout.fillWidth: true
                            from: 0.4
                            to: 0.8
                            stepSize: 0.05
                            value: dataModel.yoloThreshold
                            onMoved: {
                                var newValue = Math.round(value * 100) / 100
                                dataModel.SetYoloThreshold(newValue)
                                tcpClient.SetYoloThreshold(newValue)
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            Label {
                                text: "0.4"
                                font.pixelSize: 12
                                color: Material.hintTextColor
                            }
                            Item {
                                Layout.fillWidth: true
                            }
                            Label {
                                text: "0.8"
                                font.pixelSize: 12
                                color: Material.hintTextColor
                            }
                        }
                    }

                    // YOLO画框阈值
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        RowLayout {
                            Layout.fillWidth: true
                            Label {
                                text: qsTr("YOLO Draw Threshold:")
                                font.pixelSize: 14
                            }
                            Label {
                                text: dataModel.yoloDrawThreshold.toFixed(2)
                                font.pixelSize: 14
                                font.bold: true
                                color: Material.accent
                                Layout.fillWidth: true
                                horizontalAlignment: Text.AlignRight
                            }
                        }

                        Slider {
                            id: drawThresholdSlider
                            Layout.fillWidth: true
                            from: 0.2
                            to: 0.8
                            stepSize: 0.05
                            value: dataModel.yoloDrawThreshold
                            onMoved: {
                                var newValue = Math.round(value * 100) / 100
                                tcpClient.SetYoloDrawThreshold(newValue)
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            Label {
                                text: "0.2"
                                font.pixelSize: 12
                                color: Material.hintTextColor
                            }
                            Item {
                                Layout.fillWidth: true
                            }
                            Label {
                                text: "0.8"
                                font.pixelSize: 12
                                color: Material.hintTextColor
                            }
                        }
                    }
                }
            }

            // 视频查看区
            Pane {
                Layout.fillWidth: true
                Layout.topMargin: 8
                Layout.bottomMargin: 16
                Material.elevation: 2

                ColumnLayout {
                    width: parent.width
                    spacing: 16

                    Label {
                        text: qsTr("Video Monitoring")
                        font.pixelSize: 16
                        font.bold: true
                    }

                    Label {
                        text: qsTr("Stream Profile")
                        font.pixelSize: 14
                        font.bold: true
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        Button {
                            text: "1080p"
                            Layout.fillWidth: true
                            onClicked: tcpClient.SetStreamProfile(1080)
                        }
                        Button {
                            text: "720p"
                            Layout.fillWidth: true
                            onClicked: tcpClient.SetStreamProfile(720)
                        }
                        Button {
                            text: "480p"
                            Layout.fillWidth: true
                            onClicked: tcpClient.SetStreamProfile(480)
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Label {
                            text: qsTr("Inference Interval:")
                            font.pixelSize: 14
                        }
                        Label {
                            text: dataModel.inferInterval + qsTr(" frame(s)")
                            font.pixelSize: 14
                            font.bold: true
                            color: Material.accent
                            Layout.fillWidth: true
                            horizontalAlignment: Text.AlignRight
                        }
                    }

                    Slider {
                        Layout.fillWidth: true
                        from: 1
                        to: 6
                        stepSize: 1
                        value: dataModel.inferInterval
                        onMoved: tcpClient.SetInferInterval(Math.round(value))
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Label {
                            text: qsTr("Realtime FPS:")
                            font.pixelSize: 13
                        }
                        Label {
                            text: dataModel.streamFps.toFixed(1)
                            font.pixelSize: 13
                            font.bold: true
                            color: Material.accent
                            Layout.fillWidth: true
                            horizontalAlignment: Text.AlignRight
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Label {
                            text: qsTr("Infer Latency:")
                            font.pixelSize: 13
                        }
                        Label {
                            text: dataModel.inferMs.toFixed(1) + " ms"
                            font.pixelSize: 13
                            font.bold: true
                            color: Material.accent
                            Layout.fillWidth: true
                            horizontalAlignment: Text.AlignRight
                        }
                    }

                    Button {
                        text: qsTr("View Live Stream")
                        Layout.fillWidth: true
                        highlighted: true
                        icon.name: "video"
                        onClicked: {
                            var url = "http://" + tcpClient.serverAddress + ":80"
                            Qt.openUrlExternally(url)
                        }
                    }

                    Button {
                        text: qsTr("Open Direct MJPEG Stream")
                        Layout.fillWidth: true
                        onClicked: {
                            var url = "http://" + tcpClient.serverAddress + ":8091/?action=stream"
                            Qt.openUrlExternally(url)
                        }
                    }

                    Label {
                        text: qsTr("Click button to open stream in browser")
                        font.pixelSize: 12
                        color: Material.hintTextColor
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }
        }
    }

    // Qt虚拟键盘仅在非Android平台加载，Android使用系统原生IME不需要此组件
    Loader {
        id: inputPanelLoader
        active: Qt.platform.os !== "android"
        z: 99
        x: 0
        width: window.width
        // 根据InputPanel的active状态动态计算Loader位置
        y: active && item ? (item.active ? window.height - item.height : window.height)
                          : window.height
        Behavior on y {
            NumberAnimation { duration: 250; easing.type: Easing.InOutQuad }
        }
        sourceComponent: Component {
            InputPanel {
                width: inputPanelLoader.width
            }
        }
    }
}
