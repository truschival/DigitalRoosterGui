function add_leading_zero(var_in){
    if (var_in < 10)
        return "0"+var_in;
    return ""+var_in;
}

function display_time_ms(ms) {
    // make it full integer seconds
    var time_s = Math.floor(ms/1000);
    var ss = time_s%60;
    // time in minutes
    var time_min = (time_s-ss)/60;
    var mm = time_min%60;
    // time in hours
    var time_h = (time_min-mm)/60;

    return add_leading_zero(time_h) + ":" + add_leading_zero(mm) +":" + add_leading_zero(ss);
}

function get_hours(time){
	var hrs = Qt.formatTime(time, "hh");
	return hrs;
}

function get_minutes(time){
	var min = Qt.formatTime(time, "mm");
	return min;
}
