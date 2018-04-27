/******************************************************************************
* \filename
* \brief
*
* \details
*
* \copyright (c) 2018 2018  Thomas Ruschival <thomas@ruschival.de>
* \license {This file is licensed under GNU PUBLIC LICENSE Version 2 or later
* 			 SPDX-License-Identifier: GPL-2.0-or-later}
*
*****************************************************************************/

#include <QDebug>
#include <QMediaPlayer>

#include "alarm.hpp"
#include "alarmdispatcher.hpp"
#include "mediaplayerproxy.hpp"
#include "configuration_manager.hpp"

using namespace DigitalRooster;

/***********************************************************************/

AlarmDispatcher::AlarmDispatcher(MediaPlayerProxy* playerproxy, ConfigurationManager* confman,
	QObject* parent)  : QObject(parent), mpp(playerproxy), cm(confman) {
    qDebug() << Q_FUNC_INFO;

	QObject::connect(mpp, &MediaPlayerProxy::playback_state_changed,
		[=](QMediaPlayer::State state) { 
		if (state == QMediaPlayer::PlayingState) {
			qDebug() << " resetting player_not_started flag";
			player_not_started = false;
		}
	});

	QObject::connect(&interval_timer, SIGNAL(timeout), this, SLOT(check_alarms));
	interval_timer.start(interval);

	fallback_alarm_timer.setSingleShot(true);
	fallback_alarm_timer.setInterval(fallback_timeout);
	QObject::connect(&fallback_alarm_timer, SIGNAL(timeout), this, SLOT(fallback_triggered));
}

/*****************************************************************************/
void AlarmDispatcher::check_alarms()  {
	qDebug() << Q_FUNC_INFO;
	auto now = QDateTime::currentDateTime();
	for (const auto& alarm : cm->get_alarms()) {
		auto delta = abs(now.secsTo(alarm->get_next_trigger()));
		if (delta * 1000 < interval / 2) {
			player_not_started = false;
			mpp->set_media(alarm->get_media());
			mpp->play();
			emit alarm_triggered();
			fallback_alarm_timer.start(fallback_timeout);
		}
	}
}

/*****************************************************************************/
void AlarmDispatcher::fallback_triggered() {
	if (player_not_started) {
		qWarning() << "Player has not started in due time!";
		//TODO: mpp->set_media(fallback_media);
		mpp->play();
	}
}