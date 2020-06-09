import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2

ListView {
    boundsBehavior: Flickable.StopAtBounds
    rebound: listBoundTransition;

    delegate: WifiNetworkDelegate{
        id: wifidelegate
    }
    highlight: Rectangle { color: "lightsteelblue"; radius: 5 }

    model: wifilistmodel;
}
