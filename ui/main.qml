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
        //MiraController.setSbi1Series(sbi1Line);
        MiraController.setSbi2Series(sbi2Line);
        MiraController.setIndicationSeries(indicationLine);
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

        // --- 2. ORTA BÖLÜM (GRAFİK VE SAĞ MENÜ) ---
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 15

            // 2A. GRAFİK ALANI (SOL VE ORTA)
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "#2d2d2d"
                radius: 10

                ChartView {
                    anchors.fill: parent
                    anchors.margins: 10
                    title: "Speed - Distance Curve"
                    titleColor: "white"
                    titleFont.pixelSize: 20  // Başlık font boyutu
                    titleFont.bold: true     // İsteğe bağlı kalınlaştırma
                    backgroundColor: "transparent"
                    legend.labelColor: "white"
                    antialiasing: true

                    ValueAxis {
                        id: axisX
                        min: 0
                        max: MiraController.distanceRange
                        titleText: "<font color='white'>Distance from target (m)</font>"
                        labelsColor: "white"
                        reverse: true
                    }

                    ValueAxis {
                        id: axisY
                        min: 0
                        max: MiraController.speedRange + 20
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
                    //LineSeries {
                    //    id: sbi1Line
                    //    name: "SBI 1"
                    //    axisX: axisX; axisY: axisY
                    //    color: "#32CD32"
                    //    style: Qt.DashLine
                    //    width: 2
                    //}
                    LineSeries {
                        id: sbi2Line
                        name: "SBI"
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

            // 2B. SAĞ MENÜ (BİLGİ PANELLERİ VE FİLTRELER)
            Rectangle {
                Layout.preferredWidth: 240
                Layout.fillHeight: true
                color: "#2d2d2d"
                radius: 10

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 15

                    // --- BİLGİ PANELLERİ ---
                    Rectangle {
                        Layout.fillWidth: true
                        implicitHeight: 60
                        color: "#3d3d3d"; radius: 8
                        RowLayout {
                            anchors.fill: parent; anchors.margins: 10
                            Text { text: "Train Max\nSpeed:"; color: "#aaaaaa"; font.pixelSize: 16 }
                            Text { text: MiraController.trainMaxSpeedText; color: "#00ffcc"; font.pixelSize: 18; font.bold: true; Layout.alignment: Qt.AlignRight }
                        }
                    }
                    Rectangle {
                        Layout.fillWidth: true
                        implicitHeight: 50
                        color: "#3d3d3d"; radius: 8
                        RowLayout {
                            anchors.fill: parent; anchors.margins: 10
                            Text { text: "Speed:"; color: "#aaaaaa"; font.pixelSize: 16 }
                            Text { text: MiraController.currentSpeedText; color: "#00ffcc"; font.pixelSize: 18; font.bold: true; Layout.alignment: Qt.AlignRight }
                        }
                    }
                    Rectangle {
                        Layout.fillWidth: true
                        implicitHeight: 60
                        color: "#3d3d3d"; radius: 8
                        RowLayout {
                            anchors.fill: parent; anchors.margins: 10
                            Text { text: "Mod:"; color: "#aaaaaa"; font.pixelSize: 16 }
                            Text { text: MiraController.currentModeText; color: "#ffcc00"; font.pixelSize: 18; font.bold: true; Layout.alignment: Qt.AlignRight }
                        }
                    }
                    Rectangle {
                        Layout.fillWidth: true
                        implicitHeight: 50
                        color: "#3d3d3d"; radius: 8
                        RowLayout {
                            anchors.fill: parent; anchors.margins: 10
                            Text { text: "Level:"; color: "#aaaaaa"; font.pixelSize: 16 }
                            Text { text: MiraController.currentLevelText; color: "#ff99aa"; font.pixelSize: 18; font.bold: true; Layout.alignment: Qt.AlignRight }
                        }
                    }

                    // (Divider)
                    Rectangle {
                        Layout.fillWidth: true
                        implicitHeight: 2
                        color: "#444444"
                        Layout.topMargin: 10
                        Layout.bottomMargin: 10
                    }

                    // --- FİLTRELER ---
                    Text {
                        text: "Filters:"
                        color: "white"
                        font.pixelSize: 18
                        font.bold: true
                        Layout.bottomMargin: 5
                    }

                    CheckBox {
                        text: "EBI"
                        checked: true
                        font.pixelSize: 16
                        palette.windowText: "white"
                        onCheckedChanged: ebiLine.visible = checked
                    }
                    // CheckBox {
                        // text: "SBI 1"
                        // checked: true
                        // font.pixelSize: 16
                        // palette.windowText: "white"
                        // onCheckedChanged: sbi1Line.visible = checked
                    // }
                    CheckBox {
                        text: "SBI"
                        checked: true
                        font.pixelSize: 16
                        palette.windowText: "white"
                        onCheckedChanged: sbi2Line.visible = checked
                    }
                    CheckBox {
                        text: "Warning"
                        checked: true
                        font.pixelSize: 16
                        palette.windowText: "white"
                        onCheckedChanged: warningLine.visible = checked
                    }
                    CheckBox {
                        text: "Permitted"
                        checked: true
                        font.pixelSize: 16
                        palette.windowText: "white"
                        onCheckedChanged: permittedLine.visible = checked
                    }
                    CheckBox {
                        text: "Indication"
                        checked: true
                        font.pixelSize: 16
                        palette.windowText: "white"
                        onCheckedChanged: indicationLine.visible = checked
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        implicitHeight: 2
                        color: "#444444"
                        Layout.topMargin: 10
                        Layout.bottomMargin: 10
                    }

                    Button {
                        text: "Clear"
                        font.pixelSize: 16
                        font.bold: true
                        implicitWidth: 200
                        implicitHeight: 50
                        background: Rectangle {
                            color: parent.down ? "#0055cc" : "#0077ff"
                            radius: 5
                        }
                        contentItem: Text {
                            text: parent.text
                            color: "white"
                            font.pixelSize: 20
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        onClicked: MiraController.clearAllSeries();
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Layout.topMargin: 10 // Clear butonu ile araya mesafe
                        spacing: 10

                        // +500 Butonu
                        Button {
                            text: "+500"
                            font.pixelSize: 16
                            font.bold: true
                            Layout.fillWidth: true // Alanı kaplaması için
                            implicitHeight: 50
                            background: Rectangle {
                                color: parent.down ? "#006600" : "#008000" // Yeşil tonları
                                radius: 5
                            }
                            contentItem: Text {
                                text: parent.text
                                color: "white"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            // Önerilen Fonksiyon İsmi
                            onClicked: MiraController.increaseRange500();
                        }

                        // -500 Butonu
                        Button {
                            text: "-500"
                            font.pixelSize: 16
                            font.bold: true
                            Layout.fillWidth: true // Alanı kaplaması için
                            implicitHeight: 50
                            background: Rectangle {
                                color: parent.down ? "#cc6600" : "#e67e22" // Turuncu tonları
                                radius: 5
                            }
                            contentItem: Text {
                                text: parent.text
                                color: "white"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            // Önerilen Fonksiyon İsmi
                            onClicked: MiraController.decreaseRange500();
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Layout.topMargin: 10
                        spacing: 10

                        // +500 Butonu
                        Button {
                            text: "+50"
                            font.pixelSize: 16
                            font.bold: true
                            Layout.fillWidth: true // Alanı kaplaması için
                            implicitHeight: 50
                            background: Rectangle {
                                color: parent.down ? "#006600" : "#008000" // Yeşil tonları
                                radius: 5
                            }
                            contentItem: Text {
                                text: parent.text
                                color: "white"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            // Önerilen Fonksiyon İsmi
                            onClicked: MiraController.increaseRange50();
                        }

                        // -500 Butonu
                        Button {
                            text: "-50"
                            font.pixelSize: 16
                            font.bold: true
                            Layout.fillWidth: true // Alanı kaplaması için
                            implicitHeight: 50
                            background: Rectangle {
                                color: parent.down ? "#cc6600" : "#e67e22" // Turuncu tonları
                                radius: 5
                            }
                            contentItem: Text {
                                text: parent.text
                                color: "white"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            // Önerilen Fonksiyon İsmi
                            onClicked: MiraController.decreaseRange50();
                        }
                    }








                    // Seçenekleri yukarı itmek için boşluğu alta ekledik
                    Item { Layout.fillHeight: true }
                }
            }
        }
    }
}
