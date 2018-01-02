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

#include <PodcastSource.hpp>
#include <QString>
#include <QDate>
#include <QFile>
#include <cstddef> //size_t
#include <QXmlStreamReader>

#include "PodcastSource.hpp"
#include "PlayableItem.hpp"

namespace DigitalRooster {
/**
 * Class retrieves a podcast-rss feed from a PodcastSourceConfiguration and
 * updates a list of podcast-episodes
 */
class PodcastSourceReader {

public:
	PodcastSourceReader() = default;
	/**
	 * retrieve and parse RSS feed for general information, stores the retrieved RSS
	 * in the PodcastSourceConfiguration
	 */
	void update_podcast(DigitalRooster::PodcastSource& podcastsource);

private:
	std::vector<DigitalRooster::PodcastSource> podcastsources;
	/**
	 * Take the RSS feed and extract information and update PodcastSourceConfig
	 * @param podcastsource
	 *  @param xml open xml stream reader set to channel element of rss-feed
	 */
	void parse_episodes(DigitalRooster::PodcastSource& podcastsource,
			QXmlStreamReader& xml);

	/**
	 * Take the RSS feed and extract information and update PodcastSourceConfig
	 * @param podcastsource
	 * @param xml open xml stream reader set to channel element of rss-feed
	 */
	void parse_channel(DigitalRooster::PodcastSource& podcastsource,
			QXmlStreamReader& xml);
};
};
#endif // _PODCASTSOURCEREADER_HPP_ 
