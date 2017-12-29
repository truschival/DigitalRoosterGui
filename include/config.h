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

#include <string>
#include <vector>
#include <QDir>

namespace DigitalRooster{

	/**
	 * Application CompileTime Configurations
	 */

	/**
	 * Name of configuration file
	 */
	const std::string SYSTEM_CONFIG_PATH("/tmp/dr.ini");

	/**
	 * INI configuration Key for Sources
	 */
	const std::string KEY_GROUP_SOURCES("Sources");

	/**
	 * Internet Radio entries are found here
	 */
	const std::string KEY_GROUP_IRADIO_SOURCES("InternetRadio");

	/**
	 * Podcast feeds
	 */
	const std::string KEY_GROUP_PODCAST_SOURCES("Podcasts");

	/**
	 * Key for all URLs
	 */
	const std::string KEY_URL("url");

	/**
	 * Key for all nice names
	 */
	const std::string KEY_NAME("name");

	/**
	 * Directory for all downloaded RSS Files
	 */
	const QDir RSS_FILE_DIR("/tmp/rss/");

	/**
	 * Runtime configuration object
	 */
	class AppConfig{
	public:
		std::string mediaFile;
	};

}
