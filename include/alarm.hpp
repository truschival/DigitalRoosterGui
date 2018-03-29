/******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 2 or later
 * 			 SPDX-License-Identifier: GPL-2.0-or-later}
 *
 *****************************************************************************/

#ifndef _ALARM_HPP_
#define _ALARM_HPP_

#include <QObject>
#include <QDateTime>
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
	 * Alarm periodicity
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
			const QUrl& media, QObject* parent = nullptr);

	/**
	 * Has to delete media
	 */
	virtual ~Alarm();

	/**
	 * Query time when the alarm is to be triggered
	 * @return
	 */
	const QDateTime& get_next_trigger();

	/**
	 * set time of day when the alarm should occur
	 * @param timeofday 00:00 to 23:59
	 */
	void set_time(const QTime & timeofday);
	const QTime & get_time() const {
		return time_of_day;
	}

	/**
	 * Set repeating behavior of alarm
	 * @param period
	 */
	void set_period(Alarm::Period period);

	Alarm::Period get_period() const {
		return period;
	}

	/**
	 * enable alarm to play next time
	 * @param state
	 */
	void enable(bool state) {
		enabled = state;
	}

	bool is_enabled() const {
		return enabled;
	}
private:
	/**
	 * What to play when alarm triggers
	 */
	PlayableItem* media;

	/**
	 * when alarm is repeated
	 */
	Alarm::Period period;

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
	QDateTime next_trigger;

};

}
;
#endif // _ALARM_HPP_
