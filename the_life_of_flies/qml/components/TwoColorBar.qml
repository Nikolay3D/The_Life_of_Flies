import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {    

    property color backgroung: "#ff0000"
    property color foregroung: "#00ff00"

    id: _background
    width: 300
    height: 15
    color: backgroung

    property real val: 1
    property real max: 1

    Rectangle {
        id: _hp
        color: foregroung
        width: parent.width * val / max
        height: parent.height
    }


}
