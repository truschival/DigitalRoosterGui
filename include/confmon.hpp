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
#ifndef _CONFMON_H_
#define _CONFMON_H_

#include <QString>
#include <QSettings>

namespace DigitalRooster {

	class ConfMon : public QSettings {
		Q_OBJECT
	public:

		ConfMon();

		const QString& getCfgPath();

		void sendSignal();

		public slots:

	signals:
//		void config_changed(const AppConfig&);

	private:
		QString cfgPath;
	};
}
#endif /* _CONFMON_H_ */












