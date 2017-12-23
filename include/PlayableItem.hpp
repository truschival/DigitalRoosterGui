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

#ifndef _PLAYABLEITEM_HPP_
#define _PLAYABLEITEM_HPP_ 

#include <QUrl>
#include <QString>
#include <QDate>

/**
 * Self contained information to play
 */
class PlayableItem {

public:
	/**
	 * Convenience constructor for testing
	 * @param name
	 * @param url
	 */
	PlayableItem(const char* name, const char* url) :
			display_name(name), uri(QString(url)) {
	}
	;

	PlayableItem(const QString& name, const QUrl& url) :
			display_name(name), uri(url) {
	}
	;

	const QString& get_display_name() {
		return display_name;
	}

	const QUrl& get_url() {
		return uri;
	}

	/** default destructor */
	virtual ~PlayableItem() = default;

private:
	/**
	 * Name of Radio stream
	 */
	QString display_name;

	/**
	 * Location of stream
	 */
	QUrl uri;

};

/**
 * Media source with random access in time, player can go back and forth
 */
class SeekablePlayableItem: public PlayableItem {

public:
	SeekablePlayableItem(const QString& name, const QUrl& url) :
			PlayableItem(name, url) {
	}
	;

	/** default Destructor */
	virtual ~SeekablePlayableItem()=default;

	/**
	 * Has it been played completely?
	 */
	bool completed();
	/**
	 * Percentage of already listened content
	 */
	int get_position_percent();
	/**
	 * Percentage of already listened content
	 */
	void set_position_percent(int newVal);

private:
	/**
	 * Percentage of already listened content
	 */
	int position_percent = 0;

};

/**
 * PodcastEpisode = item of RSS feed
 */
struct PodcastEpisode: public SeekablePlayableItem {

public:
	PodcastEpisode(const QString& name, const QUrl& url) :
			SeekablePlayableItem(name, url) {
	}
	;
	virtual ~PodcastEpisode() = default;

	/**
	 * Author of the episode
	 */
	QString author;
	/**
	 * Synopsis of this episode
	 */
	QString description;
	/**
	 * Global Unique ID of item
	 */
	QString guid;
	/**
	 * Release date of episode (item)
	 */
	QDate publication_date;

};
#endif // _PLAYABLEITEM_HPP_ 
