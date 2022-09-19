import QtQuick 2.12
import BoardModelURI 1.0
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.12
import components 1.0

RowLayout {
    id: root

    property bool visible_fly_info: id_switch_info.checked
    property var cells_centers: new Map()
    property int watch_fly: -1
    property var fly_track: []

    signal add_fly_clicked(int cell_id)
    onAdd_fly_clicked: function(cell_id){
        if(!id_sb_stupidity.enabled){
            board.model.add_fly(cell_id)
        }else{
            board.model.add_fly(cell_id, id_sb_stupidity.value)
        }
    }

    signal remove_fly_clicked(int fly_id)
    onRemove_fly_clicked: function(fly_id){
        board.model.remove_fly(fly_id)
        if(fly_id === watch_fly){
            clear_watched_fly()
        }
    }

    signal watch_fly_clicked(int fly_id)
    onWatch_fly_clicked: function(fly_id){
        if(fly_id < 0){
            return
        }

        watch_fly = fly_id
        var JsonObject= JSON.parse(board.model.fly_info(fly_id));
        if(undefined === JsonObject){
            return;
        }

        id_lbl_watch_fly_id.text = fly_id
        id_lbl_watch_fly_age_limit.text = JsonObject["hp_max"] + " ms"
        id_lbl_watch_fly_stupidity.text = JsonObject["stupidity"] + " ms"
        var jumps = JsonObject["jumps"]
        id_lbl_watch_fly_jumps.text = jumps.length
        var age = JsonObject["hp_max"] - JsonObject["hp"] //[ms]
        id_lbl_watch_fly_age.text = age + " ms"
        id_lbl_watch_fly_speed.text = (jumps.length/(age/1000/60)).toFixed(2) + " jumps/min"

        draw_track(jumps);
    }

    function update_cell_center(cell_id, x, y){
        cells_centers.set(cell_id, {"x": x, "y":y})
    }

    function draw_track(cells){
        console.log("draw_track: dbg", cells)
        fly_track = cells
        var ctx = trackCanvas.getContext("2d");
        ctx.reset();
        trackCanvas.requestPaint();
        trackCanvas.visible = true
    }

    function clear_watched_fly(){
        var ctx = trackCanvas.getContext("2d");
        ctx.reset();
        trackCanvas.visible = false
        watch_fly = -1

        id_lbl_watch_fly_id.text = ""
        id_lbl_watch_fly_age_limit.text = ""
        id_lbl_watch_fly_stupidity.text = ""
        id_lbl_watch_fly_jumps.text = ""
        id_lbl_watch_fly_age.text = ""
        id_lbl_watch_fly_speed.text = ""
    }

    Rectangle {
        id: board_rect
        Layout.fillWidth: true
        Layout.fillHeight: true


        TableView {
            id: board
            anchors.fill: parent

            columnSpacing: 5
            rowSpacing: 5
            clip: true

            interactive: false

            columnWidthProvider: function (column) {
                return board.model ? (board.width/board.model.columnCount())-columnSpacing : 0
            }

            rowHeightProvider: function (row) {
                return board.model ? (board.height/board.model.rowCount())-rowSpacing : 0
            }

            onWidthChanged: board.forceLayout()
            onHeightChanged: board.forceLayout()

            model: BoardModel {
                id: board_model

                onDataChanged: {
                    watch_fly_clicked(watch_fly)
                }
            }

            delegate: FlyCell {

                visible_fly_info: root.visible_fly_info
                jsonDefinition: model.flies
                watch_fly_id: watch_fly

                implicitWidth: board.columnWidthProvider(column)
                implicitHeight: board.rowHeightProvider(row)

                Component.onCompleted: {
                    add_fly_clicked.connect(root.add_fly_clicked)
                    remove_fly_clicked.connect(root.remove_fly_clicked)
                    watch_fly_clicked.connect(root.watch_fly_clicked)
                    update_cell_center(get_cell_id(), x+width/2, y+height/2)

                }

                onWidthChanged: {
                    update_cell_center(get_cell_id(), x+width/2, y+height/2)
                }

                onHeightChanged: {
                    update_cell_center(get_cell_id(), x+width/2, y+height/2)
                }
            }
        }

        Canvas {
            id: trackCanvas
            anchors.fill: parent
            visible: false

            onPaint: {
                var context = getContext("2d");
                context.save()
                if(fly_track.length > 1){
                    context.lineWidth = 7

                    context.fill()
                    context.stroke()
                    context.beginPath()
                    var start = cells_centers.get(fly_track[0])
                    context.moveTo(start["x"], start["y"])
                    for(var j=1; j < fly_track.length; j++){
                        var end= cells_centers.get(fly_track[j])
                        context.lineTo(end["x"], end["y"])
                        context.moveTo(end["x"], end["y"])
                    }
                    context.closePath()
                    context.strokeStyle = "#4fc1e9"
                    context.lineWidth = 2;
                    context.stroke()
                }
                context.restore()
            }
        }
    }



    ColumnLayout{
        Layout.maximumWidth: 250

        GridLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true
            columns: 2

            Label {
                text: qsTr("Board dimention:")
                Layout.fillWidth: true
            }

            SpinBox {
                id: id_sb_dimention
                Layout.maximumWidth: 150
                Layout.fillWidth: true
                from: 2
                to: 7
                value: 4
                editable: true
            }

            Label {
                text: qsTr("Cell capacity:")
                Layout.fillWidth: true
            }

            SpinBox {
                id: id_sb_cell_capacity
                Layout.maximumWidth: 150
                Layout.fillWidth: true
                from: 1
                to: 9
                value: 5
                editable: true

                ToolTip.visible: hovered
                ToolTip.text: qsTr("Max number flies in the cell")
                ToolTip.delay: 500
            }

            Label {
                text: qsTr("Fly stupidity:")
                Layout.fillWidth: true
            }

            SpinBox {
                id: id_sb_stupidity
                Layout.maximumWidth: 150
                Layout.fillWidth: true
                from: 4000
                to: 8000
                value: 5000
                editable: true

                ToolTip.visible: hovered
                ToolTip.text: qsTr("Stupidity for new flies")
                ToolTip.delay: 500
            }

            Button {
                id: id_btn_create_board
                Layout.alignment: Qt.AlignHCenter
                text: "Create board"
                Layout.columnSpan: 2
                Layout.fillWidth: true
                onClicked:{
                    clear_watched_fly()
                    board.model.init(id_sb_dimention.value, id_sb_cell_capacity.value)
                    id_btn_start.text = "Start"
                }
            }

            Button {
                id: id_btn_create_random_board
                Layout.alignment: Qt.AlignHCenter
                text: qsTr("Create  random board")
                Layout.columnSpan: 2
                Layout.fillWidth: true
                onClicked:{
                    clear_watched_fly()
                    board.model.init()
                    id_btn_start.text = "Start"
                }
            }

            Button {
                id: id_btn_clear_board
                Layout.alignment: Qt.AlignHCenter
                text: qsTr("Clear board")
                Layout.columnSpan: 2
                Layout.fillWidth: true
                onClicked:{
                    clear_watched_fly()
                    board.model.clear()
                    id_btn_start.text = "Start"
                }
            }

            Button {
                id: id_btn_add_random_fly
                Layout.alignment: Qt.AlignHCenter
                text: qsTr("Add random fly")
                Layout.fillWidth: true
                Layout.columnSpan: 2
                Layout.topMargin: 20
                onClicked:{
                    board.model.add_fly()
                }
            }

            Button {
                id: id_btn_start
                Layout.alignment: Qt.AlignHCenter
                text: qsTr("Start")
                Layout.topMargin: 20
                Layout.columnSpan: 2
                Layout.fillWidth: true
                onClicked:{
                    if( "Start" === id_btn_start.text){
                        id_btn_start.text = "Pause"
                        board.model.start()
                    }else{
                        id_btn_start.text = "Start"
                        board.model.pause()
                    }
                }
            }
        }

        Frame {
            width: 200
            height: 200
            Layout.fillHeight: true
            Layout.fillWidth: true

            GridLayout {
                anchors.fill: parent
                Layout.fillHeight: true
                Layout.fillWidth: true
                columns: 2

                Label {
                    text: qsTr("Selected fly info")
                    horizontalAlignment: Text.AlignHCenter
                    Layout.fillWidth: true
                    Layout.columnSpan: 2
                }

                Label {
                    text: qsTr("Id:")
                    Layout.fillWidth: true
                }

                Label {
                    id: id_lbl_watch_fly_id
                    Layout.fillWidth: true
                }

                Label {
                    text: qsTr("Age limit:")
                    Layout.fillWidth: true
                }

                Label {
                    id: id_lbl_watch_fly_age_limit
                    Layout.fillWidth: true
                }

                Label {
                    text: qsTr("Stupidity:")
                    Layout.fillWidth: true
                }

                Label {
                    id: id_lbl_watch_fly_stupidity
                    Layout.fillWidth: true
                }

                Label {
                    text: qsTr("Age:")
                    Layout.fillWidth: true
                }

                Label {
                    id: id_lbl_watch_fly_age
                    Layout.fillWidth: true
                }

                Label {
                    text: qsTr("Jumps:")
                    Layout.fillWidth: true
                }

                Label {
                    id: id_lbl_watch_fly_jumps
                    Layout.fillWidth: true
                }

                Label {
                    text: qsTr("Speed:")
                    Layout.fillWidth: true
                }

                Label {
                    id: id_lbl_watch_fly_speed
                    Layout.fillWidth: true
                }
            }
        }

        Switch {
            id: id_switch_info
            text: qsTr("View extended info")
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Rectangle {
            color: "#00ffffff"
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}


