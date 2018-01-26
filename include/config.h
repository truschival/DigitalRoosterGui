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

#include <QString>
#include <QDir>

namespace DigitalRooster{

	/**
	 * Application CompileTime Configurations
	 */

	/**
	 * Name of configuration file
	 */
	const QString SYSTEM_CONFIG_PATH("config/digitalrooster.ini");

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
	 * Key for all URLs
	 */
	const QString KEY_URL("url");

	/**
	 * Key for all nice names
	 */
	const QString KEY_NAME("name");

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
