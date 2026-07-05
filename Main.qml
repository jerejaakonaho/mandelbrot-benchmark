import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic


Window {
    width: 1280
    height: 720
    minimumWidth: 150
    minimumHeight: 250
    visible: true
    title: qsTr("Mandelbrot Generation Benchmark")
    color: "#240C04"
    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Connections {
            target: cpuGenerator
            function onImageReady() {
                cpuMandelbrot.source = "file:mandelbrot.png?id=" + Math.random();
            }
        }

        // Navbar
        Rectangle {
            id: navbar
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            color: "transparent"
            Layout.alignment: Qt.AlignTop

            // Divider line
            Rectangle {
                width: parent.width
                height: 2
                color: "#464243"
                anchors.bottom: parent.bottom
            }


            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 20
                anchors.rightMargin: 20
                spacing: 16

                // Navbar text
                Text {
                    text: "GPU vs CPU benchmark"
                    color: "#E8E8E6"
                    font.pixelSize: 22
                    font.bold: true
                    Layout.alignment: Qt.AlignCenter
                }

                // Spacer
                Item {
                    Layout.fillWidth: true
                }
            }
        }

        // Main Content
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 20

            // GPU layout with a picture of the generated mandelbrot
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

                // Generation button
                Button {
                    Layout.topMargin: 20
                    padding: 10
                    text: "Start GPU Benchmark"
                    Layout.alignment: Qt.AlignCenter
                }

                // Image of Mandelbrot
                Image {
                    id: gpuMandelbrot
                    source: ""
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    fillMode: Image.PreserveAspectFit
                    cache: false
                }
            }

            Rectangle {
                Layout.fillHeight: true
                Layout.preferredWidth: 2
                color: "#464243"
            }

            // CPU layout with a picture of the generated mandelbrot
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

                // Generation button
                Button {
                    padding: 10
                    Layout.topMargin: 20
                    text: "Start CPU Benchmark"
                    Layout.alignment: Qt.AlignCenter
                    onClicked:  cpuGenerator.startBenchmark();
                }

                // Image of Mandelbrot
                Image {
                    id: cpuMandelbrot
                    source: ""

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    fillMode: Image.PreserveAspectFit
                    cache: false
                }
            }
        }
    }
}


