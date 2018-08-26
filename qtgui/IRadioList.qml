import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1

ListView {
    id:iradiolist
	property string objectName : "InternetRadio"
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

    delegate: IRadioDelegate{
        id: iradiodelegate
    }

    model: iradiolistmodel
}
