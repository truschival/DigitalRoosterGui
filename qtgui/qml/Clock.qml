import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1

import "Jsutil.js" as Util

Item{
	id: currentTime
	property int hours
	property int minutes
	property int seconds

	property bool night
	property string timestring_lz;
	property string timestring_lz_hh_mm;
	property string datestring_lz;

	function timeChanged() {
		var date = new Date();
		seconds = date.getSeconds()
		minutes=  date.getMinutes()
		hours = date.getHours()
		night = ( hours < 7 || hours > 19 );
		datestring_lz = date.toDateString("ddd dd.MM.yyyy");
		timestring_lz_hh_mm = Util.add_leading_zero(hours) + ":"
			+ Util.add_leading_zero(minutes) ;
		timestring_lz = timestring_lz_hh_mm + ":"
			+ Util.add_leading_zero(seconds)
	}

	Timer {
		interval: 500; running: true; repeat: true;
		onTriggered: currentTime.timeChanged()
	}
}
