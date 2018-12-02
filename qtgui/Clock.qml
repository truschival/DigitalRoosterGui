import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1

Item{
	id: currentTime
	property int hours
	property int minutes
	property int seconds

	property bool night
	property string timestring_lz;
	property string datestring_lz;

	function timeChanged() {
		var date = new Date();
		seconds = date.getSeconds()
		minutes=  date.getMinutes()
		hours = date.getHours()
		night = ( hours < 7 || hours > 19 );
		datestring_lz = date.toDateString("ddd dd.MM.yyyy");
		timestring_lz = date.toTimeString("HH:mm:ss");
	}

	Timer {
		interval: 500; running: true; repeat: true;
		onTriggered: currentTime.timeChanged()
	}
}
