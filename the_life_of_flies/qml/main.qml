import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import components 1.0


ApplicationWindow {
    visible: true
    width: 1100
    height: 815
    title: qsTr("The Life of Flies")

    FlyBoard {
        anchors.fill: parent
        anchors.margins: 5
    }

}
