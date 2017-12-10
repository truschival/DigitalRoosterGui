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
#include "config_items.h"
#include "config.h"

namespace DigitalRooster{
class ConfigurationManager
{

public:
	/**
	 * Default constructor with path to ini file
	 * @param filepath
	 */
	ConfigurationManager(const std::string& filepath=DigitalRooster::SYSTEM_CONFIG_PATH);

	virtual ~ConfigurationManager()=default;

	/**
	 * get all radio stream sources
	 */
	const std::vector<RadioStreamSource>& get_stream_sources(){
		return stream_sources;
	}

	/**
	 * Append the radio stream to list - duplicates will not be checked
	 * @param src the new stream source
	 */
	void add_radio_station(RadioStreamSource&& src);
	void add_radio_station(const RadioStreamSource& src);

	void add_podcast_feed(PodcastSource& src);

	/**
	 * Write memory config to file - will overwrite changes in file
	 */
	void write_config_file();

private:
	QSettings qs;
	std::vector<PodcastSource> m_PodcastSource;
	std::vector<RadioStreamSource> stream_sources;

	/**
	 * Fills the vector stream_sources with entries form settings file
	 */
	void read_radio_streams_from_file();


};

}//namespace
#endif // _SETTINGS_READER_HPP_ 
