/*************************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \author ruschi
 *
 * \since  20-MAr-2016 16:46:01 AM
 *
 *************************************************************************************/

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












