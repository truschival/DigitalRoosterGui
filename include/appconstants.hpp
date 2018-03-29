/*************************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \author ruschi
 *
 *************************************************************************************/

#pragma once

#include "config.h"
#include <QString>
#include <QDir>

namespace DigitalRooster{
	/**
	 * Name of configuration file
	 */
	const QString SYSTEM_CONFIG_PATH(QString(SETTINGS_PATH) + "/" + QString(SETTINGS_FILE_NAME));

	/**
	 * INI configuration Key for Sources
	 */
	const QString KEY_GROUP_SOURCES("Sources");

	/**
	 * Internet Radio entries are found here
	 */
	const QString KEY_GROUP_IRADIO_SOURCES("InternetRadio");

	/**
	 * Podcast feeds
	 */
	const QString KEY_GROUP_PODCAST_SOURCES("Podcasts");

	/**
	 * Key for all URIs
	 */
	const QString KEY_URI("uri");

	/**
	 * Key for all nice names
	 */
	const QString KEY_NAME("name");

	/**
	* Keyword for perdiodic refresh 
	*/
	const QString KEY_UPDATE_INTERVAL("UpdateInterval");

	/**
	 * Directory for all downloaded RSS Files
	 */
	const QString RSS_FILE_DIR(QDir::tempPath());

	/**
	 * Runtime configuration object
	 */
	class AppConfig{
	public:
		std::string mediaFile;
	};

}
