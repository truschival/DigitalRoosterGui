import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3


Rectangle{
    width: parent.width;
    height: Style.contentHeight/4;
    radius: 3;
    border.width: 1;
    color: connected ? 	Style.colors.enabled : Style.colors.disabled;

    MouseArea {
        anchors.fill: parent
        onPressAndHold:  {
            wifilistmodel.currentIndex =index;
            console.log("Wifi pressed : "+index);
            wifilistmodel.wps_connect(index);
        }
    }
    RowLayout{
        anchors.fill: parent
        anchors.margins: Style.itemMargins.medium;
        spacing: Style.itemSpacings.medium;

        IconLabel {
            text: icon_for_signal(signal);
            Layout.alignment: Qt.AlignCenter | Qt.AlignVCenter
        }

        Text {
            id: network_name;
            text: name;
            font: Style.font.label;
            elide: Text.ElideRight;
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter;
            Layout.maximumWidth: parent.width*0.8;
        }
    }

    function icon_for_signal(sig){
        console.log("icon_for_signal " + sig);
        if (sig < -90){

            console.log("sig gone " + sig);
            return "\uf92e"
        } else if  (sig < -80){

            console.log("sig fair " + sig);
            return "\uf91e";
        }  else if  (sig < -70){

            console.log("sig medium " + sig);
            return "\uf921";
        } else if  (sig < -60){
            console.log("sig good " + sig);
            return "\uf924";
        }
        return "\uf927"; // full
    }
}
