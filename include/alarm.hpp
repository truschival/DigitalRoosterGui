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
	Q_PROPERTY(bool enabled READ is_enabled WRITE enable NOTIFY enabled_changed)
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
     * One-shot alarm for a exact DateTime to trigger
     * @param media_url what to play
     * @param exact time instance
     * @param enabled activated/deactivated
     * @param the obligatory QObject parent
     */
    Alarm(const QUrl& media_url, const QDateTime& timepoint,
        bool enabled = true,
        QObject* parent = nullptr);

    /**
     * Construct an alarm for given time
     * @param media what to play
     * @param timepoint time of day - any time of day
     * @param period periodicity
     * @param enabled activated/deactivated
     * @param the obligatory QObject parent
     */
    Alarm(const QUrl& media, const QTime& timepoint,
        Alarm::Period period = Alarm::Daily, bool enabled = true,
        QObject* parent = nullptr);

	/**
	 * Need Default constructor to register with QML
	 */
	Alarm():media(nullptr){};
		
	/**
	 * next trigger instant when the alarm is to be triggered
	 * @return
	 */
	const QDateTime& get_next_trigger();

	/**
	 * set time of day when the alarm should occur
	 * @param timeofday 00:00 to 23:59
	 * @param period periodicity
	 */
	void set_trigger(const QTime & timeofday, Alarm::Period period);
	
	/**
	 * Set the alarm to trigger once at given timeinsance
	 * @param timeinstance when to trigger alarm
	 */
	void set_trigger(const QDateTime & timeinstance);

	/**
	 * @return time of day when alarm is due
	 */
	const QTime get_time() const {
		return trigger_instant.time();
	}
	/**
	 * Set repeating behavior of alarm
	 * @param period
	 */
	Alarm::Period get_period() const {
		return period;
	}

	/**
	 * Change periodicity
	 */
	void set_period(Alarm::Period period) {
		this->period = period;
		update_trigger();
	};

	/**
	 * Alarm media
	 */
	std::shared_ptr<PlayableItem> get_media() const {
		return media;
	}

	/**
	* is this alarm set
	* @return state
	*/
	bool is_enabled() const {
		return enabled;
	}

	/**
	 * enable alarm to play next time
	 * @param state
	 */
	public slots:	  
		void enable(bool state) {
			enabled = state;
		}

	signals:
		void enabled_changed(bool state);

private:
	/**
	 * What to play when alarm triggers
	 */
	std::shared_ptr<PlayableItem> media;

	/**
	 * when alarm is repeated
	 */
	Alarm::Period period;

	/**
	 * Time point when this alarm will trigger
	 * The date part of is ignored except for period "Once"
	 */
	QDateTime trigger_instant;

	/**
	 * Will it trigger?
	 */
	bool enabled;

	/**
	 * Calculate the upcoming timepoint when the alarm should trigger
	 */
	void update_trigger();
};

}
;
#endif // _ALARM_HPP_
