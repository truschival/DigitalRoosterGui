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

#include <config.hpp>
#include <QString>

namespace DigitalRooster {

class ConfMon{
public:
	ConfMon() : cfgPath(SYSTEM_CONFIG_PATH.c_str()){};

	const QString& getCfgPath(){
		return cfgPath;
	}

private:
	QString cfgPath;

};

}
#endif /* _CONFMON_H_ */












