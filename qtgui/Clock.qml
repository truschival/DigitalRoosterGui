import QtQuick 2.6
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.1
import QtQml 2.2

Item{
	id: currentTime
	property var locale: Qt.locale("DE_de")
	property int hours
	property int minutes
	property int seconds

	property bool night
	property string timestring_lz;
	property string datestring_lz;

	function leadingZero(value){
		var ret = ""
		if (value < 10){
			ret = "0" + value
			return ret
		} else {
			ret = value
		}
		return ret
	}

	function timeChanged() {
		var date = new Date;
		seconds = date.getUTCSeconds()
		minutes=  date.getMinutes()
		hours = date.getHours()
		night = ( hours < 7 || hours > 19 );
		datestring_lz = date.toLocaleDateString(locale, "ddd dd.MM.yyyy");
		timestring_lz = date.toLocaleTimeString(locale, "HH:mm:ss");
	}

	Timer {
		interval: 500; running: true; repeat: true;
		onTriggered: currentTime.timeChanged()
	}
}
