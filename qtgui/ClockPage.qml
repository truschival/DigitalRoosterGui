import QtQuick 2.6
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.1

Page {
    Label{
        x: 307
        y: 0
        text: currentTime.timestring_lz
        anchors.verticalCenterOffset: -60
        font.pointSize: 36
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
    }
     Label{
        x: 307
        y: 0
        text: currentTime.datestring_lz
        anchors.verticalCenterOffset: 60
        font.pointSize: 24
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
    }
}
