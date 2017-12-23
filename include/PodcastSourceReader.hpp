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

namespace DigitalRooster{
/**
 * Class retrieves a podcast-rss feed from a PodcastSourceConfiguration and
 * updates a list of podcast-episodes
 */
class PodcastSourceReader
{

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
	PodcastSourceConfiguration create_PodcastSource(const QString& display_name, const QString& url, size_t max_episodes = 0);

	/**
	 * Download RSS feed an store locally
	 * @param url
	 * @return path to xml file
	 */
	QString download_rss(const QString& url);

	void parse_episodes(const QString& rss_feed);

	void parse_info(const QString& rss_feed);
	/**
	 * Read max_episodes form file
	 */
	void update_episodes(PodcastSourceConfiguration& config);
	/**
	 * retrieve and parse RSS feed for general information, stores the retrieved RSS
	 * in the PodcastSourceConfiguration
	 */
	void update_info(PodcastSourceConfiguration& config);

private:
	std::vector<PodcastSourceConfiguration> podcastsources;

};
};
#endif // _PODCASTSOURCEREADER_HPP_ 
