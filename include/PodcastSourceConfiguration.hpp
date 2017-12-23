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

#ifndef _PODCASTSOURCECONFIGURATION_HPP_
#define _PODCASTSOURCECONFIGURATION_HPP_ 

#include <QString>
#include <QDate>
#include <QFile>
#include <QMap>
#include <cstddef> //size_t
#include <memory>
#include "PlayableItem.hpp"

namespace DigitalRooster{
/**
 * Class to represent a RSS channel with items as episodes
 */
class PodcastSourceConfiguration {

public:
	PodcastSourceConfiguration(const QString &url) :
			rss_feed_uri(url) {
	}
	;

	/**
	 * Add an episode to episodes
	 */
	void add_episode(std::shared_ptr<PodcastEpisode>  episode);

	/**
	 * Deletes all episodes from list
	 */
	void flush_episodes();

	/**
	 * Description of the Channel (mandatory by RSS2.0 spec)
	 */
	const QString& get_description() {
		return description;
	}

	/**
	 * When was this podcast source last scanned for new items
	 */
	const QDate& get_last_updated() {
		return last_updated;
	}
	;

	/**
	 * Website of RSS feed channel (not the rss xml URI but additional information)
	 */
	const QString& get_link() {
		return link;
	}

	/**
	 * show max_episodes in the list
	 */
	size_t get_max_episodes() {
		return max_episodes;
	}
	;

	/**
	 * path local XML file for RSS feed
	 */
	const QString& get_rss_feed() {
		return rss_file;
	}

	/**
	 * title element of RSS channel
	 */
	const QString& get_title() {
		return title;
	}

	/**
	 * seconds until update is meaningful. Calculated from optional items skip_days
	 * and skip_hours
	 */
	int get_update_period() {
		return update_period;
	}
	;
	/**
	 * URL for rss feed of this podcast
	 */
	const QString& get_url() {
		return rss_feed_uri;
	}

	/**
	 * Remove an episode from the list of episodes
	 * @param name episode to remove
	 */
	void remove_episode(const QString& name);

	/**
	 * Description of the Channel (mandatory by RSS2.0 spec)
	 */
	void set_description(QString newVal) {
		description = newVal;
	}

	/**
	 * When was this podcast source last scanned for new items
	 */
	void set_last_updated(QDate& newVal) {
		last_updated = newVal;
	}

	/**
	 * Website of RSS feed channel (not the rss xml URI but additional information)
	 */
	void set_link(QString newVal) {
		link = newVal;
	}

	/**
	 * set number of displayed/downloaded episodes
	 */
	void set_max_episodes(size_t max) {
		max_episodes = max;
	}

	/**
	 * local XML file last read from internet
	 */
	void set_rss_feed(QString filepath) {
		rss_file = filepath;
	}

	/**
	 * title element of RSS channel
	 * \param newTitle
	 */
	void set_title(QString newTitle) {
		title = newTitle;
	}

	/**
	 * seconds until update is meaningful. Calculated from optional items skip_days
	 * and skip_hours
	 * \param period new period
	 */
	void set_update_period(int period) {
		update_period = period;
	}

private:
	/**
	 * Description of the Channel (mandatory by RSS2.0 spec)
	 */
	QString description;
	/**
	 * Map of episodes
	 */
	QMap<QString, std::shared_ptr<PodcastEpisode>> episodes;
	/**
	 * When was this podcast source last scanned for new items
	 */
	QDate last_updated;
	/**
	 * Website of RSS feed channel (not the rss xml URI but additional information)
	 */
	QString link;
	/**
	 * show max_episodes in the list
	 */
	size_t max_episodes=5;
	/**
	 * path local XML file for RSS feed
	 */
	QString rss_file;

	/**
	 * title element of RSS channel
	 */
	QString title;

	/**
	 * seconds until update is meaningful. Calculated from optional items skip_days
	 * and skip_hours
	 */
	int update_period=0;

	/**
	 * URL for rss feed of this podcast channel
	 */
	QString rss_feed_uri;

};
};
#endif // _PODCASTSOURCECONFIGURATION_HPP_ 
