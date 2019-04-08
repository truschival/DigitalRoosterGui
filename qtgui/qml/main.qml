import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1
import QtMultimedia 5.9
import ruschi.PodcastEpisode 1.0

import "." // QTBUG-34418, singletons require explicit import to load qmldir file
import "Icon.js" as MdiFont
import "Jsutil.js" as Util


ApplicationWindow {
    id: applicationWindow
    visible: true
    width: Style.canvasWidth;
    height: Style.canvasHeight;

    property alias playerControlWidget: playerControlWidget
    property string functionMode: "Clock"

    Clock{
        id: currentTime
    }

    FontLoader {
        id: materialdesignIconsFont;
        source: "materialdesignicons-webfont.ttf"
    }

    header: ToolBar {
        height: Style.toolbarHeight;

        RowLayout {
            anchors.fill: parent
            anchors.margins: Style.itemMargins.medium;
            anchors.topMargin: Style.itemMargins.slim;

            spacing: Style.itemSpacings.dense;

            IconButton {
                text: MdiFont.Icon.menu
                onClicked: {
                    drawer.open()
                }
            }
            Label {
                id: titleLabel
                text: (stackView.depth > 1) ? currentTime.timestring_lz_hh_mm : "";
                font: Style.font.title;
                elide: Label.ElideRight
                Layout.fillWidth: true
            }

            
            IconLabel{
                text:"\uf51a"
                visible: (playerProxy.playbackState == MediaPlayer.PlayingState)
                color: "white"
            }
            
            Label {
                id: countdown_to_sleep
                text: sleeptimer.time_remaining;
                Layout.rightMargin: Style.itemMargins.medium;
                font: Style.font.title;
                visible: (playerProxy.playbackState == MediaPlayer.PlayingState)
            }
            

            IconButton {
                id : playerControlBtn
                text: MdiFont.Icon.play
                onClicked:{
                    playerControlWidget.show()
                }
            }

            IconButton {
                id : backButton
                text: MdiFont.Icon.keyboardBackspace
                Layout.rightMargin: Style.itemMargins.wide;
                visible: (stackView.depth > 1)
                onClicked:{
                    stackView.backNavigate()
                }

                Shortcut {
                    sequence: StandardKey.Back
                    onActivated: backButton.onClicked();
                }
            }
        }
    }

    Drawer {
        id: drawer
        width: Style.drawer.w;
        height: applicationWindow.height
        interactive: true;
        margins: Style.itemMargins.slim;
        edge: Qt.LeftEdge;

        ListView {
            id: listView
            anchors.fill: parent
            spacing: Style.itemSpacings.dense;
            anchors.margins: Style.itemMargins.slim;
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            focus: true;
            currentIndex: -1;

            delegate: IconButton {
                text: model.title
                highlighted: listView.currentIndex == index

                onClicked: {
                    console.log("Current "+ listView.currentIndex +
                                " index: "+index + " depth:"+ stackView.depth)
                    if( stackView.depth > 1){
                        stackView.pop(null)
                    }
                    listView.currentIndex = index
                    drawer.close()
                    /* Special item: power off button */
                    if(index === listView.count -1 ){
                        console.log("last item!")
                        powerOffMenu.popup((applicationWindow.width-powerOffMenu.width)/2,
                                           (applicationWindow.height-powerOffMenu.height)/2 - Style.itemMargins.extrawide)
                        return; // nothing to do
                    }
                    stackView.push(model.source)
                }
            }

            model: ListModel {
                ListElement { title: "\uf223"; source: "qrc:/PodcastList.qml"; objectName:"PodcastList"; }
                ListElement { title: "\uf43B"; source: "qrc:/IRadioList.qml"; objectName:"InternetRadio"; }
                ListElement { title: "\uf020"; source: "qrc:/AlarmList.qml"; objectName:"AlarmList"; }
                ListElement { title: "\uf62e"; source: "qrc:/SettingsPage.qml"; objectName:"Settings"; }
                ListElement { title: "\uf425"; source: ""; objectName:"PowerOff"; }
            }
        }
    }

    Menu {
        id: powerOffMenu
        GridLayout {
            rows: 2;
            columns: 2;
            rowSpacing: Style.itemSpacings.medium;
            columnSpacing: Style.itemSpacings.medium;

            IconButton {
                id: poweroffBtn
                text: "\uf901"
                Layout.alignment: Qt.AlignCenter | Qt.AlignVCenter
                onClicked: {
                    console.log("power off button")
                    powerControl.power_off();
                }
            }
            Text{
                text: "shutdown";
                font: Style.font.boldLabel;
                color: "white"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }


            IconButton {
                id: rebootBtn
                text: "\uf900";
                Layout.alignment: Qt.AlignCenter | Qt.AlignVCenter
                onClicked: {
                    console.log("reboot button")
                    powerControl.reboot();
                }
            }
            Text{
                text: "reboot";
                font: Style.font.boldLabel;
                color: "white"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }
        }
    }

    PlayerControlWidget{
        id: playerControlWidget
        width: parent.width*0.85;
        x: Math.round((applicationWindow.width - width) / 2)
        y: Math.round((applicationWindow.height - height) *0.6)
    }

    VolumePopup{
        id: volumePopUp
        x: Math.round((applicationWindow.width - width) / 2)
        y: Math.round((applicationWindow.height - height) / 2)
    }

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: ClockPage{
            id:initalClockPage
            property string objectName : "InitialPage"
        }

        function backNavigate(){
            if (stackView.depth > 1){
                stackView.pop()
            } else{
                stackView.currentIndex = -1;
            }
        }

        function reset(){
            while(stackView.depth >1){
                stackView.pop(null);
            }
        }
    }

    /**** global connections ****/
    Component.onCompleted: {
        console.log("main.qml completed")
        powerControl.going_in_standby.connect(stackView.reset)
        volumeButton.volume_incremented.connect(volumePopUp.show)
    }
} // application window
