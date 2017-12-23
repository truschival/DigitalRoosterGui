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

#ifndef _PODCASTSOURCEREADER_HPP_
#define _PODCASTSOURCEREADER_HPP_ 

#include <QString>
#include <QDate>
#include <QFile>
#include <cstddef> //size_t
#include <QXmlStreamReader>

#include "PlayableItem.hpp"
#include "PodcastSourceConfiguration.hpp"

namespace DigitalRooster {
/**
 * Class retrieves a podcast-rss feed from a PodcastSourceConfiguration and
 * updates a list of podcast-episodes
 */
class PodcastSourceReader {

public:
	PodcastSourceReader();

	virtual ~PodcastSourceReader();

	/**
	 * Create PodcastEpisode from a RSS Feed Item
	 */
	PodcastEpisode create_PodcastEpisode(QXmlStreamReader& xml);
	/**
	 * Create a Podcast Sourceconfiguration from a URL with name and max_episodes
	 */
	PodcastSourceConfiguration create_PodcastSource(const QString& display_name,
			const QString& url, size_t max_episodes = 0);

	/**
	 * Download RSS feed an store locally
	 * @param url
	 * @return path to xml file
	 */
	QString download_rss(const QString& url);

	/**
	 * Read max_episodes form file
	 */
	void update_episodes(PodcastSourceConfiguration& podcastsource);

	/**
	 * retrieve and parse RSS feed for general information, stores the retrieved RSS
	 * in the PodcastSourceConfiguration
	 */
	void update_podcast(PodcastSourceConfiguration& podcastsource);

private:
	std::vector<PodcastSourceConfiguration> podcastsources;
	/**
	 * Take the RSS feed and extract information and update PodcastSourceConfig
	 * @param podcastsource
	 *  @param xml open xml stream reader set to channel element of rss-feed
	 */
	void parse_episodes(PodcastSourceConfiguration& podcastsource,
			QXmlStreamReader& xml);

	/**
	 * Take the RSS feed and extract information and update PodcastSourceConfig
	 * @param podcastsource
	 * @param xml open xml stream reader set to channel element of rss-feed
	 */
	void parse_channel(PodcastSourceConfiguration& podcastsource,
			QXmlStreamReader& xml);
};
}
;
#endif // _PODCASTSOURCEREADER_HPP_ 
