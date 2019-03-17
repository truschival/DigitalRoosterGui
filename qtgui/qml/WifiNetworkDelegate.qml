import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3


Rectangle{
    width: parent.width;
    height: Style.contentHeight/4;
    radius: 3;
    color: connected ? 	Style.colors.enabled : Style.colors.disabled;

    MouseArea {
	anchors.fill: parent
	onPressAndHold:  {
	    wifilistmodel.currentIndex =index;
	    console.log("Wifi pressed : "+index);
	}
    }
    RowLayout{
	anchors.fill: parent
	anchors.margins: Style.itemMargins.slim;
	spacing: Style.itemSpacings.medium;

	Text {
	    id: network_name;
	    text: name;
       	    Layout.fillWidth: true;
	    font: Style.font.label;
	}

	Text {
	    id: signal_strength;
	    font: Style.font.boldLabel;
	    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter;
	    text: signal;
	}
    }

}
