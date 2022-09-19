import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import components 1.0
import "ColorInterpolation"
import "CircularProgressbar"

Rectangle {
    id: root
    width: 50
    height: 50

    property int hp_width: 2

    property int uniq_id
    property real hp: 0.5
    property real hp_max: 1
    property int type: 1

    property real stupid: 5000
    property real stupid_min: 4000
    property real stupid_max: 8000

    property bool visible_info: false
    property bool mark: false

    signal click_delete
    signal click_watch

    signal hovered
    signal unhovered

    function getImage(id) {
        var out = "../../img/fly_unknown.png"
        switch (id){
        case 1:
            out = "../../img/fly__1.png"
            break
        case 2:
            out = "../../img/fly__2.png"
            break
        case 3:
            out = "../../img/fly__3.png"
            break
        case 4:
            out = "../../img/fly__4.png"
            break
        case 5:
            out = "../../img/fly__5.png"
            break
        case 6:
            out = "../../img/fly__6.png"
            break
        case 7:
            out = "../../img/fly__7.png"
            break
        }
        return out;
    }

    Rectangle {
        ColorInterpolation {
            id: interpolation
            stops: [
                InterpolationStop { position: 0.0; color: "grey" },
                InterpolationStop { position: 0.25; color: "blue" },
                InterpolationStop { position: 0.5; color: "green" },
                InterpolationStop { position: 0.75; color: "yellow" },
                InterpolationStop { position: 1.0; color: "red" }
            ]
        }

        id: stupidity_background
        width: parent.width < parent.height ? parent.width : parent.height
        height: width
        anchors.centerIn: parent
        radius: width < height ? width : height
        color: interpolation.getColorAt((stupid-stupid_min)/(stupid_max-stupid_min))
        opacity: 0.5

    }

    CircularProgressBar {
        id: progress
        anchors.centerIn: parent
        lineWidth: hp_width
        value: root.hp/root.hp_max
        size: stupidity_background.width < stupidity_background.height ? stupidity_background.width : stupidity_background.height
        secondaryColor: "#FF6B50"
        primaryColor: "#4BEF83"
        visible: root.hp > 0 ? true : false
    }

    Image {
        id: image
        anchors.fill: parent
        source: getImage(root.type)
        fillMode: Image.PreserveAspectFit
        sourceSize.height: root.width - hp_width*2
    }

    Image {
        id: image_top
        anchors.fill: parent
        source: "../../img/fly-swatter.png"
        fillMode: Image.PreserveAspectFit
        sourceSize.height: root.width - hp_width*2
        visible: root.hp > 0 ? false : true
    }

    RowLayout {
        id: action_rect
        anchors.fill: parent
        spacing: 0

        property color bg_hovered: "#C0C0C0C0"

        Rectangle {
            id: action_delete
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "transparent"

            Image {
                id: image_remove
                source: "../../img/delete.png"
                fillMode: Image.PreserveAspectFit
                sourceSize.height: parent.width
                anchors.centerIn: parent
                visible: false
            }

            MouseArea {
                z: 100
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor

                onEntered: {
                    parent.color = action_rect.bg_hovered
                    image_remove.visible = true
                    hovered()
                }

                onExited: {
                    parent.color = "transparent"
                    image_remove.visible = false
                    unhovered()
                }

                onClicked: {
                    click_delete()
                }
            }
        }

        Rectangle {
            id: action_watch
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "transparent"

            Image {
                id: image_watch
                source: "../../img/eye2.png"
                fillMode: Image.PreserveAspectFit
                sourceSize.height: parent.width
                anchors.centerIn: parent
                visible: false
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor

                onEntered: {
                    parent.color = action_rect.bg_hovered
                    image_watch.visible = true
                    hovered()
                }

                onExited: {
                    parent.color = "transparent"
                    image_watch.visible = false
                    unhovered()
                }

                onClicked: {
                    click_watch()
                }
            }

        }
    }

    Rectangle {
        id: id_marker
        anchors.fill: parent
        color: "#00FFFFFF"
        border.color: "#4fc1e9"
        border.width: 2
        visible: mark
    }

    Label {
        visible: visible_info
        height: root.hp_width
        text: root.stupid
        anchors.bottom: parent.bottom
    }
}
