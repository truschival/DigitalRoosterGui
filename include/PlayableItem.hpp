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
#include <QDateTime>
#include <QDebug>

namespace DigitalRooster{
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
	/**
	 * Constructor
	 * @param name episode title
	 * @param url source uri
	 * @param episode_guid (optinal, if none given URL will serve as GUID)
	 */
	PodcastEpisode(const QString& name, const QUrl& url, const QString & episode_guid="") :
			SeekablePlayableItem(name, url), guid(episode_guid) {

			if(guid.isEmpty()){
				guid=url.toString();
			}
	}
	;
	virtual ~PodcastEpisode() = default;


	bool operator ==(const PodcastEpisode& rhs){
		return this->guid == rhs.guid;
	}

	/**
	 * Global Unique ID of item
	 */
	QString guid;
	/**
	 * Release date of episode (item)
	 */
	QDateTime publication_date;
	/**
	 * Author of the episode
	 */
	QString author;
	/**
	 * Synopsis of this episode
	 */
	QString description;


};
};
#endif // _PLAYABLEITEM_HPP_ 
