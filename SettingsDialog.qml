import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

Dialog {
    id: settingsDialog
    title: qsTr("Settings")
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    
    width: Math.min(parent.width * 0.9, 400)
    height: Math.min(parent.height * 0.8, 500)
    
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    
    Material.elevation: 8
    
    required property var settingsManager
    
    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        
        Pane {
            Layout.fillWidth: true
            Material.elevation: 1
            
            ColumnLayout {
                width: parent.width
                spacing: 12
                
                Label {
                    text: qsTr("Language")
                    font.pixelSize: 16
                    font.bold: true
                    color: Material.primary
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12
                    
                    RadioButton {
                        id: chineseRadio
                        text: "简体中文"
                        checked: settingsManager.language === "zh_CN"
                        onClicked: settingsManager.SetLanguage("zh_CN")
                    }
                    
                    RadioButton {
                        id: englishRadio
                        text: "English"
                        checked: settingsManager.language === "en_US"
                        onClicked: settingsManager.SetLanguage("en_US")
                    }
                }
                
                Label {
                    text: qsTr("Restart required for language change to take effect")
                    font.pixelSize: 11
                    color: Material.color(Material.Orange)
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                    visible: settingsManager.language !== "zh_CN"
                }
            }
        }
        
        Pane {
            Layout.fillWidth: true
            Layout.topMargin: 12
            Material.elevation: 1
            
            ColumnLayout {
                width: parent.width
                spacing: 12
                
                Label {
                    text: qsTr("Connection")
                    font.pixelSize: 16
                    font.bold: true
                    color: Material.primary
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    
                    Switch {
                        id: autoConnectSwitch
                        checked: settingsManager.autoConnect
                        onToggled: settingsManager.SetAutoConnect(checked)
                    }
                    
                    Label {
                        text: qsTr("Auto-connect on startup")
                        font.pixelSize: 14
                        Layout.fillWidth: true
                    }
                }
                
                Label {
                    text: qsTr("Automatically connect to server when app starts")
                    font.pixelSize: 11
                    color: Material.hintTextColor
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                    leftPadding: autoConnectSwitch.width
                }
            }
        }
        
        Pane {
            Layout.fillWidth: true
            Layout.topMargin: 12
            Material.elevation: 1
            
            ColumnLayout {
                width: parent.width
                spacing: 12
                
                Label {
                    text: qsTr("Display")
                    font.pixelSize: 16
                    font.bold: true
                    color: Material.primary
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    
                    Switch {
                        id: keepScreenOnSwitch
                        checked: settingsManager.keepScreenOn
                        onToggled: settingsManager.SetKeepScreenOn(checked)
                    }
                    
                    Label {
                        text: qsTr("Keep screen on")
                        font.pixelSize: 14
                        Layout.fillWidth: true
                    }
                }
                
                Label {
                    text: qsTr("Prevent screen from turning off while app is running")
                    font.pixelSize: 11
                    color: Material.hintTextColor
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                    leftPadding: keepScreenOnSwitch.width
                }
            }
        }
        
        Item {
            Layout.fillHeight: true
        }
        
        Pane {
            Layout.fillWidth: true
            Material.elevation: 0
            background: Rectangle {
                color: Material.color(Material.Grey, Material.Shade100)
                radius: 4
            }
            
            ColumnLayout {
                width: parent.width
                spacing: 6
                
                Label {
                    text: qsTr("About")
                    font.pixelSize: 12
                    font.bold: true
                    color: Material.hintTextColor
                }
                
                Label {
                    text: qsTr("Intelligent Monitoring System v0.1")
                    font.pixelSize: 11
                    color: Material.hintTextColor
                }
                
                Label {
                    text: "© 2026 MoeControl"
                    font.pixelSize: 11
                    color: Material.hintTextColor
                }
            }
        }
    }
    
    onAccepted: {
        settingsManager.SaveSettings()
    }
    
    onRejected: {
        settingsManager.LoadSettings()
    }
}
