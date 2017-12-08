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

#endif // _CONFIG_ITEMS_H_
