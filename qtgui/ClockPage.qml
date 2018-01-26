import QtQuick 2.6
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.1

Page {
    id: page

    Item{
        id: currentTime
        property int hours
        property int minutes
        property int seconds
        property bool night
        property string datetime;

        function timeChanged() {
            var date = new Date;
            datetime = Date().toString()
            hours = date.getHours()
            night = ( hours < 7 || hours > 19 )
            minutes = date.getMinutes()
            seconds = date.getUTCSeconds();
        }

        Timer {
            interval: 100; running: true; repeat: true;
            onTriggered: currentTime.timeChanged()
        }
    }

    Label{
        x: 307
        y: 0

        text: currentTime.hours+":"+currentTime.minutes+":"+currentTime.seconds
        anchors.verticalCenterOffset: -60
        font.pointSize: 19
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Label {
        x: 307
        y: 2
        text: currentTime.datetime
        font.pointSize: 19
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: 60
        anchors.horizontalCenter: parent.horizontalCenter
    }
}
