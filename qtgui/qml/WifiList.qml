import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

ListView {
    antialiasing: true
    boundsBehavior: Flickable.StopAtBounds
    maximumFlickVelocity: 1500
    flickDeceleration: 1500
    //snapMode: ListView.SnapOneItem

    delegate: WifiNetworkDelegate{
        id: wifidelegate
    }
    move: Transition {
        NumberAnimation { properties: "x,y"; duration: 1000 }
    }
    highlight: Rectangle { color: "lightsteelblue"; radius: 5 }

    model: wifilistmodel;
}
