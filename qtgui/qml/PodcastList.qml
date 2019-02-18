import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.1

ListView {
    id: podcastlist
    property string objectName : "PodcastList"
    width: stackView.width
    height: stackView.height
    antialiasing: true
    highlightRangeMode: ListView.ApplyRange
    boundsBehavior: Flickable.StopAtBounds
    maximumFlickVelocity: 1500
    flickDeceleration: 1800
    snapMode: ListView.SnapOneItem
    contentWidth: stackView.width
    focus: true

    delegate: PodcastDelegate{
        id: podcastdelegate
    }

    model: podcastmodel
    onCurrentItemChanged: console.log(currentItem.display_name + ' selected')


    Menu {
	id: podcastControl
	RowLayout {
	    spacing: Style.itemSpacings.medium;
	    IconButton {
		id: refreshBtn
		text: "\uf450"
		Layout.alignment: Qt.AlignCenter | Qt.AlignVCenter
		onClicked: {
		    console.log("refresh")
		    podcastlist.model.refresh(index);
		}
	    }

	    IconButton {
		id: updateBtn; 
		text: "\uf6af";
		Layout.alignment: Qt.AlignCenter | Qt.AlignVCenter
		onClicked: {
		    console.log("update")
		    //podcastlist.model.update();
		}
	    }

	    DelayButton{
		delay:1000;

		contentItem: Text{
		    text: "\ufa79"
		    color: "white"
		    font.pointSize: 24
		    horizontalAlignment: Text.AlignHCenter
    		    font.family: "Material Design Icons"
		}

		onActivated:{
		    console.log("Deleting idx: " + index)
		 }
	    }
	}
    }
}
