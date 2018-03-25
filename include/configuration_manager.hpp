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
#include <QString>
#include <QVector>
#include <memory>

#include "appconstants.hpp"
#include "PlayableItem.hpp"
#include "PodcastSource.hpp"

namespace DigitalRooster {

class ConfigurationManager {
public:
    /**
     * Default constructor with path to ini file
     * @param filepath
     */
    ConfigurationManager(const QString& filepath = DigitalRooster::SYSTEM_CONFIG_PATH);

    virtual ~ConfigurationManager() = default;

    /**
     * get all radio stream sources
     */
    const QVector<std::shared_ptr<RadioStream>>& get_stream_sources() {
        return stream_sources;
    }

    /**
     * get all radio stream sources
     */
    const QVector<std::shared_ptr<PodcastSource>>& get_podcast_sources() {
        return podcast_sources;
    }
    /**
     * Append the radio stream to list - duplicates will not be checked
     * @param src the new stream source - we take ownership
     */
    void add_radio_station(std::unique_ptr<RadioStream> src);

    /**
     * Write memory config to file - will overwrite changes in file
     */
    void write_config_file();

private:
	/**
	 * Path for configuration file
	 */
	QString filepath;
	/** 
	 * JSon Object containing the configuration including podcasts, Internet radio etc
	 * read form digitalrooster.json
	 */
	QJsonObject appconfig;

    /**
     * Internet radio stream souces are directly read form INI file
     */
    QVector<std::shared_ptr<RadioStream>> stream_sources;

    /**
     * Podcast sources (only pretty name and feed-url)
     */
    QVector<std::shared_ptr<PodcastSource>> podcast_sources;

	/**
	 * read Json file and fill sources
	 */
	void readJson();
    /**
     * Fills the vector stream_sources with entries form settings file
     */
    void read_radio_streams_from_file();

    /**
     * Read all podcast sources form configuration file
     */
    void read_podcasts_from_file();
};

} // namespace
#endif // _SETTINGS_READER_HPP_
