import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1

Rectangle{
    id: alarmDelegate
	width: parent.width;
    height: 60;
    radius: 3;
	border.width: 1;

	gradient: Gradient {
        GradientStop { position: 0.0; color: "#673AB7" }
        GradientStop { position: 0.7; color: "#3F51B5" }
		GradientStop { position: 1.0; color: "#607D8B" }
    }
	
	Text {
        id: period
        width: parent.width*0.8;
		height: parent.height/2.5;
        text: periodicity;
		anchors.top: parent.top;
		anchors.topMargin: 3;
        font.pointSize: 12;
		elide: Text.ElideRight
	}

	Text {
		id: alarmtime;
		width: parent.width*0.8;
		height: parent.height/2.5;
		anchors.bottom: parent.bottom;
		anchors.bottomMargin: 3;
        font.pointSize: 12;
		font.bold: true;
		text: triggerTime + "Uhr"
		elide: Text.ElideLeft
	}

	Switch{
		id: enaAlarm;
		anchors.top: parent.top;
		anchors.right: parent.right;
		anchors.margins: 5
		position: alarmEnabled;
		text: alarmEnabled ? qsTr("enabled") : qsTr("disabled")
	}
	    
}


