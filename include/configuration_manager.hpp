/*************************************************************************************
 * \filename    
 * \brief
 *
 * \details
 * 
 * \author ruschi
 * 
 *
 *************************************************************************************/

#ifndef _CONFIGURATION_MANAGER_H_
#define _CONFIGURATION_MANAGER_H_ 

#include <QSettings>
#include <vector>
#include <string>
#include <memory>

#include "PlayableItem.hpp"
#include "PodcastSourceConfiguration.hpp"
#include "config.h"

namespace DigitalRooster {
class ConfigurationManager {

public:
	/**
	 * Default constructor with path to ini file
	 * @param filepath
	 */
	ConfigurationManager(const std::string& filepath =
			DigitalRooster::SYSTEM_CONFIG_PATH);

	virtual ~ConfigurationManager() = default;

	/**
	 * get all radio stream sources
	 */
	const std::vector<std::shared_ptr<PlayableItem>>& get_stream_sources() {
		return stream_sources;
	}

	/**
	 * Append the radio stream to list - duplicates will not be checked
	 * @param src the new stream source - we take ownership
	 */
	void add_radio_station(std::unique_ptr<PlayableItem> src);

	/**
	 * Write memory config to file - will overwrite changes in file
	 */
	void write_config_file();

private:
	QSettings qs;

	/**
	 * Internet radio stream souces are directly read form INI file
	 */
	std::vector<std::shared_ptr<PlayableItem>> stream_sources;

	/**
	 * Podcast sources (only pretty name and feed-url)
	 */
	std::vector<std::shared_ptr<PodcastSourceConfiguration>> podcast_sources;

	/**
	 * Fills the vector stream_sources with entries form settings file
	 */
	void read_radio_streams_from_file();

	/**
	 * Read all podcast sources form configuration file
	 */
	void read_podcasts_from_file();
};

} //namespace
#endif // _SETTINGS_READER_HPP_ 
