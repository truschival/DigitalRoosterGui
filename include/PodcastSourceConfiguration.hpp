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

#include "PlayableItem.hpp"

class PodcastSourceConfiguration
{

public:
	PodcastSourceConfiguration();
	virtual ~PodcastSourceConfiguration();

	/**
	 * Add an episode to episodes
	 */
	void add_episode(PodcastEpisode episodode);
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
	int remove_episode(const QString& name);

private:
	/**
	 * Pretty name for humans
	 */
	const QString display_name;
	/**
	 * Map of episodes
	 */
	QMap<QString, PodcastEpisode> episodes;
	/**
	 * When was this podcast source last scanned for new items
	 */
	QDate last_updated;
	/**
	 * show max_episodes in the list
	 */
	size_t max_episodes;
	/**
	 * seconds until update is meaningful. Calculated from optional items skip_days
	 * and skip_hours
	 */
	int update_period;
	/**
	 * URL for rss feed of this podcast
	 */
	QString url;

};
#endif // _PODCASTSOURCECONFIGURATION_HPP_ 
