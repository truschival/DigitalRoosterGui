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

#include <QObject>
#include <QTime>
#include <memory>
namespace DigitalRooster {

class PlayableItem;

/**
 * A single alarm
 */
class Alarm: public QObject {
Q_OBJECT
public:
	/**
	 * Alarm settings periodicity
	 */
	enum Period {
		Once = 1, //!< next time the time of day occurs (within 24 hrs)
		Daily = 2, //!< every day
		Weekend = 3, //!< Saturdays & Sundays
		Workdays = 4 //!< Monday through Friday
	};

	/**
	 * Default construct an Alarm
	 * @param period periodicity
	 * @param timepoint time of day
	 * @param enabled activated/deactivated
	 * @param media what to play
	 * @param the obligatory QObject parent
	 */
	Alarm(Alarm::Period period, const QTime& timepoint, bool enabled,
			std::shared_ptr<PlayableItem> media, QObject* parent = nullptr);

	/**
	 * No special destructor needed
	 */
	~Alarm() = default;

	/**
	 * Query time when the alarm is to be triggered
	 * @return
	 */
	const QTime& get_next_trigger();

	/**
	 * set time of day when the alarm should occur
	 * @param timeofday 00:00 to 23:59
	 */
	void set_time(const QTime & timeofday);
	const QTime & get_time() const{
		return time_of_day;
	}

	/**
	 * Set repeating behavior of alarm
	 * @param period
	 */
	void set_period(Alarm::Period period);

	Alarm::Period get_period() const{
		return period;
	}

	/**
	 * enable alarm to play next time
	 * @param state
	 */
	void enable(bool state){
		enabled = state;
	}

	bool is_enabled()const{
		return enabled;
	}
private:
	/**
	 * What to play when alarm triggers
	 */
	std::shared_ptr<PlayableItem> media;

	/**
	 * when alarm is repeated
	 */
	Period period;

	/**
	 * Time point when this alarm will trigger
	 */
	QTime time_of_day;

	/**
	 * Will it trigger?
	 */
	bool enabled;

	/**
	 * Next activation of this alarm
	 */
	QTime next_trigger;

};

}
;
#endif // _ALARMS_HPP_ 
