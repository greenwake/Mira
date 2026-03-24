import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15
import QtCharts 2.15

Window {
    width: 1920
    height: 1080
    visible: true
    visibility: Window.FullScreen
    title: qsTr("Mira Dashboard")
    color: "#1e1e1e"

    Component.onCompleted: {
        MiraController.setEbiSeries(ebiLine);
        MiraController.setPermittedSeries(permittedLine);
        MiraController.setWarningSeries(warningLine);
        MiraController.setSbi1Series(sbi1Line);
        MiraController.setSbi2Series(sbi2Line);
        MiraController.setIndicationSeries(indicationLine);
        //MiraController.setEoaSeries(eoaSeries);
        //MiraController.setSvlSeries(svlSeries);
        MiraController.startSystem();
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        RowLayout {
            Layout.fillWidth: true
            Text {
                text: "ETCS Dashboard"
                color: "white"
                font.pixelSize: 24
                font.bold: true
                Layout.fillWidth: true
            }
            Button {
                text: "X"
                font.pixelSize: 16
                font.bold: true
                implicitWidth: 40
                implicitHeight: 40
                background: Rectangle {
                    color: parent.down ? "#cc0000" : "#ff3333"
                    radius: 5
                }
                contentItem: Text {
                    text: parent.text
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: Qt.quit()
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#2d2d2d"
            radius: 10

            ChartView {
                anchors.fill: parent
                anchors.margins: 10
                title: "Hız - Mesafe Eğrisi"
                titleColor: "white"
                backgroundColor: "transparent"
                legend.labelColor: "white"
                antialiasing: true

                ValueAxis {
                    id: axisX
                    min: 0
                    max: 12000
                    titleText: "<font color='white'>Distance from target (m)</font>"
                    labelsColor: "white"
                    reverse: true
                }

                ValueAxis {
                    id: axisY
                    min: 0
                    max: 250
                    titleText: "<font color='white'>Speed (km/h)</font>"
                    labelsColor: "white"
                }

                LineSeries {
                    id: ebiLine
                    name: "EBI"
                    axisX: axisX; axisY: axisY
                    color: "#00008B"
                    width: 3
                }
                LineSeries {
                    id: sbi1Line
                    name: "SBI 1"
                    axisX: axisX; axisY: axisY
                    color: "#32CD32"
                    style: Qt.DashLine
                    width: 2
                }
                LineSeries {
                    id: sbi2Line
                    name: "SBI 2"
                    axisX: axisX; axisY: axisY
                    color: "#4169E1"
                    style: Qt.DashLine
                    width: 2
                }
                LineSeries {
                    id: warningLine
                    name: "Warning"
                    axisX: axisX; axisY: axisY
                    color: "#FFA500" // Turuncu
                    width: 3
                }
                LineSeries {
                    id: permittedLine
                    name: "Permitted"
                    axisX: axisX; axisY: axisY
                    color: "#A9A9A9"
                    width: 3
                }
                LineSeries {
                    id: indicationLine
                    name: "Indication"
                    axisX: axisX; axisY: axisY
                    color: "#FFFF00" // Sarı
                    width: 3
                }

                //ScatterSeries {
                //    id: eoaSeries
                //    name: "EOA Target"
                //    axisX: axisX; axisY: axisY
                //    color: "#FF0000"
                //    markerSize: 5
                //}
                //ScatterSeries {
                //    id: svlSeries
                //    name: "SVL Target"
                //    axisX: axisX; axisY: axisY
                //    color: "#FFA500"
                //    markerSize: 5
                //}
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            spacing: 20

            RowLayout {
                spacing: 15
                Text {
                    text: "Eğri Filtreleri:"
                    color: "white"
                    font.pixelSize: 16
                    font.bold: true
                }
                CheckBox {
                    text: "EBI"
                    checked: true
                    contentItem: Text { text: parent.text; color: "white"; font.pixelSize: 16 }
                    onCheckedChanged: ebiLine.visible = checked
                }
                CheckBox {
                    text: "SBI 1"
                    checked: true
                    contentItem: Text { text: parent.text; color: "white"; font.pixelSize: 16 }
                    onCheckedChanged: sbi1Line.visible = checked
                }
                CheckBox {
                    text: "SBI 2"
                    checked: true
                    contentItem: Text { text: parent.text; color: "white"; font.pixelSize: 16 }
                    onCheckedChanged: sbi2Line.visible = checked
                }
                CheckBox {
                    text: "Warning"
                    checked: true
                    contentItem: Text { text: parent.text; color: "white"; font.pixelSize: 16 }
                    onCheckedChanged: warningLine.visible = checked
                }
                CheckBox {
                    text: "Permitted"
                    checked: true
                    contentItem: Text { text: parent.text; color: "white"; font.pixelSize: 16 }
                    onCheckedChanged: permittedLine.visible = checked
                }
                CheckBox {
                    text: "Indication"
                    checked: true
                    contentItem: Text { text: parent.text; color: "white"; font.pixelSize: 16 }
                    onCheckedChanged: indicationLine.visible = checked
                }
                //CheckBox {
                //    text: "EOA"
                //    checked: true
                //    contentItem: Text { text: parent.text; color: "white"; font.pixelSize: 16 }
                //    onCheckedChanged: eoaSeries.visible = checked
                //}
                //CheckBox {
                //    text: "SVL"
                //    checked: true
                //    contentItem: Text { text: parent.text; color: "white"; font.pixelSize: 16 }
                //    onCheckedChanged: svlSeries.visible = checked
                //}
            }

            Item {
                Layout.fillWidth: true
            }

            RowLayout {
                spacing: 15

                Rectangle {
                    implicitWidth: 160; implicitHeight: 50
                    color: "#3d3d3d"; radius: 8
                    RowLayout {
                        anchors.fill: parent; anchors.margins: 10
                        Text { text: "Hız:"; color: "#aaaaaa"; font.pixelSize: 16 }
                        Text { text: MiraController.currentSpeedText; color: "#00ffcc"; font.pixelSize: 18; font.bold: true; Layout.alignment: Qt.AlignRight }
                    }
                }
                Rectangle {
                    implicitWidth: 160; implicitHeight: 50
                    color: "#3d3d3d"; radius: 8
                    RowLayout {
                        anchors.fill: parent; anchors.margins: 10
                        Text { text: "Mod:"; color: "#aaaaaa"; font.pixelSize: 16 }
                        Text { text: "Shunting"; color: "#ffcc00"; font.pixelSize: 18; font.bold: true; Layout.alignment: Qt.AlignRight }
                    }
                }
                Rectangle {
                    implicitWidth: 160; implicitHeight: 50
                    color: "#3d3d3d"; radius: 8
                    RowLayout {
                        anchors.fill: parent; anchors.margins: 10
                        Text { text: "Level:"; color: "#aaaaaa"; font.pixelSize: 16 }
                        Text { text: MiraController.currentLevelText; color: "#ff99aa"; font.pixelSize: 18; font.bold: true; Layout.alignment: Qt.AlignRight }
                    }
                }
            }
        }
    }
}
