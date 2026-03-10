import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15
import QtCharts 2.15

Window {
    width: 1920
    height: 1080
    visible: true
    visibility: Window.FullScreen // Uygulamayı tam ekran başlatır
    title: qsTr("Mira Dashboard")
    color: "#1e1e1e" // Modern koyu tema

    Component.onCompleted: {
        // QML yüklendiğinde serileri C++ tarafına bağla ve sistemi başlat
        MiraController.setEbiSeries(ebiLine);
        MiraController.setPermittedSeries(permittedLine);
        MiraController.startSystem();
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        // ÜST BAR: Başlık ve Kapatma Butonu
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

                // Kırmızı çıkış butonu stili
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

                onClicked: Qt.quit() // Uygulamayı kapatır
            }
        }

        // ORTA ALAN: Grafik
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
                    reverse: true // Mesafe 0'a doğru gidiyor
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
                    axisX: axisX
                    axisY: axisY
                    color: "#0055ff" // Canlı Mavi
                    width: 3
                }

                LineSeries {
                    id: permittedLine
                    name: "Permitted"
                    axisX: axisX
                    axisY: axisY
                    color: "#a0a0a0" // Açık Gri
                    width: 3
                }
            }
        }

        // ALT BAR: Filtreler (Sol) ve Bilgi Panelleri (Sağ)
        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            spacing: 20

            // SOL ALT: Grafik Filtreleri
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
                    text: "Permitted"
                    checked: true
                    contentItem: Text { text: parent.text; color: "white"; font.pixelSize: 16 }
                    onCheckedChanged: permittedLine.visible = checked
                }
            }

            // Ortadaki boşluğu doldurarak sağ ve sol panelleri iki uca yaslar
            Item {
                Layout.fillWidth: true
            }

            // SAĞ ALT: Bilgi Panelleri (Dummy Veriler)
            RowLayout {
                spacing: 15

                // Hız Paneli
                Rectangle {
                    implicitWidth: 160; implicitHeight: 50
                    color: "#3d3d3d"; radius: 8
                    RowLayout {
                        anchors.fill: parent; anchors.margins: 10
                        Text { text: "Hız:"; color: "#aaaaaa"; font.pixelSize: 16 }
                        Text { text: "120 km/h"; color: "#00ffcc"; font.pixelSize: 18; font.bold: true; Layout.alignment: Qt.AlignRight }
                    }
                }

                // Mod Paneli
                Rectangle {
                    implicitWidth: 160; implicitHeight: 50
                    color: "#3d3d3d"; radius: 8
                    RowLayout {
                        anchors.fill: parent; anchors.margins: 10
                        Text { text: "Mod:"; color: "#aaaaaa"; font.pixelSize: 16 }
                        Text { text: "Shunting"; color: "#ffcc00"; font.pixelSize: 18; font.bold: true; Layout.alignment: Qt.AlignRight }
                    }
                }

                // Level Paneli
                Rectangle {
                    implicitWidth: 160; implicitHeight: 50
                    color: "#3d3d3d"; radius: 8
                    RowLayout {
                        anchors.fill: parent; anchors.margins: 10
                        Text { text: "Level:"; color: "#aaaaaa"; font.pixelSize: 16 }
                        Text { text: "Level 0"; color: "#ff99aa"; font.pixelSize: 18; font.bold: true; Layout.alignment: Qt.AlignRight }
                    }
                }
            }
        }
    }
}
