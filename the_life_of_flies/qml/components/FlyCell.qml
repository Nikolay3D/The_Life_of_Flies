import QtQuick 2.12
import QtQuick.Layouts 1.15
import QtQuick.Controls.Material 2.12
import QtQuick.Controls 2.15
import QtQml.Models 2.12
import components 1.0

Rectangle {
    id: id_root
    width: 200
    height: 250

    border {
        color: "#aab2bd"
        width: 2
    }

    property int fly_dimention: 50
    property int fly_margin: 10
    property int footer_height: 20
    property string jsonDefinition: ""
    property bool visible_fly_info: false
    property int watch_fly_id: -1

    property bool block_cell_click: false   // MouseArea ячейки находится поверх делегата. Приходится так обрабатывать клик по делегату

    QtObject {
        id: private_data
        property int cell_id: 1
        property int fullness: 1
        property int capacity: 1
        property string jsonDefinition: ""
    }

    function get_cell_id(){
        return private_data.cell_id;
    }

    signal add_fly_clicked(int cell_id)
    signal remove_fly_clicked(int fly_id)
    signal watch_fly_clicked(int fly_id)

    function updateModel(jsonString) {
        if(private_data.jsonDefinition === jsonString){
            return;
        }

        flies_model.clear()
        var JsonObject= JSON.parse(jsonString);
        if(undefined === JsonObject){
            return;
        }

        private_data.cell_id = JsonObject["cell_id"];
        private_data.fullness = JsonObject["fullness"];
        private_data.capacity = JsonObject["capacity"];

        for(var i = 0; i < private_data.fullness; i++){
            var key = "fly_" + i;
            if(undefined !== JsonObject[key]){
                flies_model.append({
                                       "fly_id": JsonObject[key].uniq_id,
                                       "fly_hp": JsonObject[key].hp,
                                       "fly_hp_max": JsonObject[key].hp_max,
                                       "fly_type": JsonObject[key].type,
                                       "fly_stupidity": JsonObject[key].stupidity
                                   })
            }
        }

        private_data.jsonDefinition = jsonString;
    }

    // модель для отображения мух в ячейке в виде сетки
    ListModel {
        id: flies_model
    }


    ColumnLayout {
        id: columnLayout
        anchors.fill: parent
        anchors.margins: id_root.border.width+2
        spacing: 0
        clip: true

        GridView {
            id: gridView
            interactive: false  // отключение прокрутки
            Layout.fillHeight: true
            Layout.fillWidth: true
            cellHeight: id_root.fly_dimention + id_root.fly_margin
            cellWidth: id_root.fly_dimention + id_root.fly_margin

            MouseArea {
                anchors.fill: parent
                z: 1
                propagateComposedEvents: true   // чтобы клик доходил до делегата
                cursorShape: Qt.PointingHandCursor
                onClicked: (mouse)=>{
                    mouse.accepted = false    // чтобы клик доходил до делегата
                    // в этом обработчике событие не останавливается
                    // а проваливается дальше

                   if(!id_root.block_cell_click){
                       // отслеживается наведение курсора на муху, чтобы при клике обрабатывать событие не в ячейке, а только в делегате
                       id_root.add_fly_clicked(get_cell_id())
                   }

                }

            }

            delegate:
                Fly{
                    height: id_root.fly_dimention
                    width: id_root.fly_dimention
                    visible_info: id_root.visible_fly_info
                    uniq_id: fly_id
                    mark: id_root.watch_fly_id === uniq_id
                    hp: fly_hp
                    hp_max: fly_hp_max
                    type: fly_type
                    stupid: fly_stupidity

                    onClick_delete: {
                        id_root.remove_fly_clicked(uniq_id)
                    }
                    onClick_watch: {
                        id_root.watch_fly_clicked(uniq_id)
                    }

                    onHovered: {
                        id_root.block_cell_click = true
                    }

                    onUnhovered: {
                        id_root.block_cell_click = false
                    }
                }

            model: flies_model



        }// GridView

        RowLayout {
            id: id_footer_layout
            Layout.fillHeight: false
            Layout.preferredHeight: id_root.footer_height
            Layout.fillWidth: true
            spacing: 0
            visible: id_root.visible_fly_info

            Label {
                id: id_lbl_number                
                text: qsTr("#"  + private_data.cell_id)
                color: "gray"
                font.pixelSize: height-4
                verticalAlignment: Text.AlignVCenter
                Layout.maximumWidth: height*2
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Label {
                id: id_lbl_capacity
                text: qsTr(private_data.fullness + "/" + private_data.capacity)
                color: "gray"
                font.pixelSize: height-4
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

        } // RowLayout


    }//ColumnLayout

    onJsonDefinitionChanged: {
        updateModel(id_root.jsonDefinition);
    }

}

