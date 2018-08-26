import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1

Page {
	id: clockPage
	property string objectName : "ClockPage"
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
   
    Label{
        x: 307
        y: 120
        text: weather.temperature
        anchors.verticalCenterOffset: 70
        font.pointSize: 24
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
    }
}
