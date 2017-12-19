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

#ifndef _CONFIG_ITEMS_H_
#define _CONFIG_ITEMS_H_

#include <QString>

class PodcastSource {

public:
	/**
	 * Pretty name for humans
	 */
	const QString& get_display_name() {
		return display_name;
	}
	/**
	 * show max_episodes in the list
	 */
	int get_max_episodes() {
		return max_episodes;
	}

	/**
	 * URL for rss feed of this podcast
	 */
	const QString& get_url() {
		return url_string;
	}

private:
	/**
	 * Pretty name for humans
	 */
	const QString display_name;
	/**
	 * show max_episodes in the list
	 */
	int max_episodes;
	/**
	 * URL for rss feed of this podcast
	 */
	const QString url_string;

};

/**
 * Internet radio stream meta information
 */
class RadioStreamSource {

public:

	RadioStreamSource(const QString& name, const QString& url) :
			display_name(name), url_string(url) {
	};

	virtual ~RadioStreamSource()= default;

	const QString& get_display_name() {
		return display_name;
	}

	const QString& get_url() {
		return url_string;
	}

private:
	/**
	 * Nice name for humans to read
	 */
	QString display_name;

	/**
	 * stream url
	 */
	QString url_string;

};

class PodcastSourceConfiguration
{

public:
	PodcastSourceConfiguration();
	virtual ~PodcastSourceConfiguration();

	/**
	 * Pretty name for humans
	 */
	const QString& get_display_name(){

		return  NULL;
	}
	/**
	 * show max_episodes in the list
	 */
	int get_max_episodes(){

		return 0;
	}
	/**
	 * URL for rss feed of this podcast
	 */
	const QString& get_url(){

		return  NULL;
	}
	/**
	 * Add an episode to episodes
	 */
	void add_episode(PodcastEpisode episodode);
	int remove_episode(const QString& name);

private:
	/**
	 * Pretty name for humans
	 */
	const QString display_name;
	/**
	 * show max_episodes in the list
	 */
	size_t max_episodes;
	/**
	 * URL for rss feed of this podcast
	 */
	QString url;
	/**
	 * Map of episodes
	 */
	QMap<QString, PodcastEpisode> episodes;
	/**
	 * When was this podcast source last scanned for new items
	 */
	QDate last_updated;
	/**
	 * seconds until update is meaningful. Calculated from optional items skip_days
	 * and skip_hours
	 */
	int update_period;

};

/**
 * Internet radio stream meta information
 */
class StreamSourceConfiguration : public PlayableItem
{

public:
	StreamSourceConfiguration();

	StreamSourceConfiguration(const QString& name, const QString& url)
	    : display_name(name),URL(url)
	{

	}
	virtual ~StreamSourceConfiguration();

};

#endif // _CONFIG_ITEMS_H_



/*************************************************************************************/
void PodcastSourceConfiguration::add_episode(PodcastEpisode episodode){

}


/*************************************************************************************/
int PodcastSourceConfiguration::remove_episode(const QString& name){

	return 0;
}