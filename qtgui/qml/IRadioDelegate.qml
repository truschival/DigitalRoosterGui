import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1

import ruschi.PlayableItem 1.0

Rectangle{
    id: iradiodelegatearea
    width: parent.width;
    height: Style.contentHeight/4;
    radius: 3;
    border.width: 1;
    color: Material.background;

    Text {
        id: stationName
        text: station_name;
        elide: Text.ElideRight
        font: Style.font.listItemHead;
        verticalAlignment: Text.AlignVCenter;
        anchors.fill: parent;
        anchors.margins: Style.itemMargins.medium;
        color: iradiolist.currentItem === parent ?
            Material.accent : "white" ;
    }

    MouseArea {
        anchors.fill: parent
        pressAndHoldInterval: applicationWindow.pressAndHoldInterval;
        onPressAndHold:{
            iradiolist.currentIndex = index;
            iradiolistmodel.send_to_player(index);
            playerControlWidget.setCurrentMediaTitle(station_name);
            playerControlWidget.show();
        }
    }
}
