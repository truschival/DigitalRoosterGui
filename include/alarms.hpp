/*************************************************************************************
 * \filename    
 * \brief
 *
 * \details
 * 
 * \author ruschi
 * 
 *
 *************************************************************************************/

#ifndef _ALARMS_HPP_
#define _ALARMS_HPP_ 

namespace DigitalRooster {
/**
 * Alarm settings periodicity
 */
enum AlarmPeriod {
	OneShot = 1, Daily = 2, Weekend = 3, WorkDays = 4
};

struct AlarmSetting {
	AlarmPeriod period;
	int time;

};
}
;
#endif // _ALARMS_HPP_ 
