import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtMultimedia 5.9
import ruschi.PodcastEpisode 1.0

import "." // QTBUG-34418, singletons require explicit import to load qmldir file
import "Jsutil.js" as Util


ApplicationWindow {
    id: applicationWindow
    visible: true
    width: Style.canvasWidth;
    height: Style.canvasHeight;

    Material.theme: Material.Dark
    Material.accent: Material.Red

    property alias playerControlWidget: playerControlWidget
    property string functionMode: "Clock"

    Clock{
        id: currentTime
    }

    FontLoader {
        id: materialdesignIconsFont;
        source: "materialdesignicons-webfont.ttf"
    }

    Timer {
        // Timer to reset stackview after player has started
        id: viewResetTimer
        interval: 60000; // 1 minute
        running: false;
        repeat: false;
        onTriggered: stackView.reset();
    }

    menuBar: ToolBar {
        height: Style.toolbarHeight;

        RowLayout {
            anchors.fill: parent
            anchors.margins: Style.itemMargins.slim;
            anchors.topMargin: 0;
            spacing: Style.itemSpacings.dense;

            IconButton {
                text: "\uf35c";
                Layout.maximumWidth: 48;
                onClicked: {
                    drawer.open()
                }
            }
            Label {
                id: titleLabel
                text: (stackView.depth > 1) ? currentTime.timestring_lz_hh_mm : "";
                font: (playerProxy.playbackState === MediaPlayer.PlayingState) ? Style.font.title : Style.font.titleBold;
                elide: Label.ElideRight
                Layout.fillWidth: true
            }

            Label{
                id: countdown_to_sleep;
                text: "<span style = 'font-family: materialdesignicons; font-size: 16pt; font-weight: bold'>\uf51a</span>
                       <span style = 'font-family: DejaVu Sans Condensed Bold, sans-serif; font-size: 16pt; font-weight: normal'>"+sleeptimer.time_remaining+"</span>"
                Layout.rightMargin: 0;
                textFormat: Text.RichText
                color: "white"
                visible: (playerProxy.playbackState === MediaPlayer.PlayingState)

                MouseArea{
                    anchors.fill: parent
                    // long click opens menu
                    onPressAndHold: {
                        sleepTimeoutMenu.popup((applicationWindow.width- sleepTimeoutMenu.width)/2,
                                               (applicationWindow.height- sleepTimeoutMenu.height)/2
                                               - Style.itemMargins.extrawide);
                    }
                    // click resets timer
                    onPressed: sleeptimer.reset_timer();

                }
            }


            IconButton {
                id : playerControlBtn
                text: "\uf40a"
                Layout.maximumWidth: 48;
                onClicked:{
                    playerControlWidget.show()
                }
            }

            IconButton {
                id : backButton
                text: "\uf30d"
                Layout.maximumWidth: 48;
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
        margins: Style.itemMargins.slim;
        closePolicy : Popup.CloseOnPressOutside;
        edge: Qt.LeftEdge;
        interactive: true;

        onOpened :{
        	autocloseTimer.start()
        }

        Timer {
	        id: autocloseTimer
	        interval: 4000
	        running: true
	        repeat: false
	        onTriggered: drawer.close();
	    }

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

    PowerMenu {
        id: powerOffMenu;
        title:"Power";
    }

    BrightnessMenu{
        id: brightnessMenu;
        title: "Brightness";
    }

    WifiMenu{
        id: wifiMenu;
        title: "Wifi";
        height: applicationWindow.height*0.8;
        width: applicationWindow.width*0.8;
    }

    SleepTimeoutMenu{
        id: sleepTimeoutMenu;
        title: "Sleep Timeout";
        height: applicationWindow.height*0.6;
        width: applicationWindow.width*0.7;
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
        anchors.fill: parent;

        initialItem: ClockPage{
            id:initalClockPage
            property string objectName : "InitialPage"
        }

        function backNavigate(){
            if (stackView.depth > 1){
                stackView.pop()
            } else{
                stackView.currentIndex = -1;
                listView.currentIndex = -1;
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
