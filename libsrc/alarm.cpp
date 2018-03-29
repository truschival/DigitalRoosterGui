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

#include <QObject>
#include "alarm.hpp"
#include "PlayableItem.hpp"
using namespace DigitalRooster;

Alarm::Alarm(Alarm::Period period, const QTime& timepoint, bool enabled,
		const QUrl& media, QObject* parent) :
		QObject(parent), media(new PlayableItem("Alarm", media)), period(period), time_of_day(
				timepoint), enabled(enabled) {

}
/*****************************************************************************/
Alarm::~Alarm(){
	delete media;
}

/*****************************************************************************/

const QDateTime & Alarm::get_next_trigger(){
	auto now = QTime();
	switch(period){
	case Alarm::Once:
		break;
	case Alarm::Weekend:
		break;
	case Alarm::Workdays:
		break;
	case Alarm::Daily:
		break;
	}
	return next_trigger;
}

/*****************************************************************************/

