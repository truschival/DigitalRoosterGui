import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.4
import QtQuick.Controls 2.2
import QtQuick.Extras 1.4
import ruschi.Alarm 1.0
import "Jsutil.js" as Util

Popup {
    property Alarm currentAlarm;
    property int index;
    modal: true;
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    enter: dialogFadeInTransition;
    exit: dialogFadeOutTransition;

    contentItem: GridLayout {
        columnSpacing: Style.itemSpacings.slim;
        rowSpacing: 0;
        anchors.fill: parent;
        anchors.margins: Style.itemMargins.slim;
        rows: 4;
        columns:2;

        Tumbler{
            id: timeTumbler
            Layout.maximumHeight: 100
            Layout.rowSpan: 2
            Layout.alignment: Qt.AlignLeft| Qt.AlignTop

            TumblerColumn {
                id: hoursTumbler
                model: 24
                width: 46;
                delegate: Text {
                    text: styleData.value
                    font: Style.font.tumbler;
                    horizontalAlignment: Text.AlignHCenter
                    opacity: 0.4 + Math.max(0, 1 - Math.abs(styleData.displacement)) * 0.6
                }
            }
            TumblerColumn {
                id: minutesTumbler
                model: 60
                width:  46;
                delegate: Text {
                    text: styleData.value
                    font: Style.font.tumbler;
                    horizontalAlignment: Text.AlignHCenter
                    opacity: 0.4 + Math.max(0, 1 - Math.abs(styleData.displacement)) * 0.6
                }
            }
        } //Tumbler

        Switch{
            id: enaAlarm;
            Layout.alignment: Qt.AlignLeft| Qt.AlignTop
            position: currentAlarm.enabled
            text: currentAlarm.enabled ? qsTr("on") : qsTr("off")
        }

        ComboBox {
            id: period
            Layout.alignment: Qt.AlignLeft| Qt.AlignTop
            model: ListModel {
                id: model
                ListElement { text: qsTr("Once") }
                ListElement { text: qsTr("Daily") }
                ListElement { text: qsTr("Weekend") }
                ListElement { text: qsTr("Workdays") }
            }
            currentIndex: currentAlarm.period_id;
        }

        ComboBox {
            id: stations
            Layout.preferredWidth: parent.width
            Layout.alignment: Qt.AlignLeft| Qt.AlignTop
            Layout.columnSpan: 2

            model: iradiolistmodel
            textRole: "station_name";
        }

        Button{
            id: alarmEditOK;
            text: qsTr("Ok");
            font: Style.font.label;
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop;
            onClicked: {
                // Time from tumbler
                var now = new Date();
                var h_idx =timeTumbler.currentIndexAt(0);
                var m_idx = timeTumbler.currentIndexAt(1);
                now.setHours(h_idx, m_idx, 0);
                currentAlarm.time = now;
                alarmlistmodel.update_row(alarmlistmodel.currentIndex);
                // update station
                currentAlarm.url = iradiolistmodel.get_station_url(stations.currentIndex);
                // alarm period
                currentAlarm.period_id = period.currentIndex;
                // enabled?
                currentAlarm.enabled= enaAlarm.position
                // update list view
                alarmlistmodel.update_row(alarmlistmodel.currentIndex)
                close();
            }
        }

        Button{
            id: alarmEditCancel;
            text: qsTr("Cancel");
            font: Style.font.label;
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop;
            onClicked: {
                close();
            }
        }
    } // Gridlayout

    onAboutToShow : {
        timeTumbler.setCurrentIndexAt(0,Util.get_hours(currentAlarm.time))
        timeTumbler.setCurrentIndexAt(1,Util.get_minutes(currentAlarm.time))

        for (var i=0; i<iradiolistmodel.rowCount() ; i++){
            if(iradiolistmodel.get_station_url(i) === currentAlarm.url){
                stations.currentIndex = i;
                break;
            }
        }
    }
}
