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

#include "QMediaContent.hpp"

/**
 * Self contained information to play
 */
class PlayableItem
{

public:
	PlayableItem();
	virtual ~PlayableItem();
	QMediaContent *m_QMediaContent;

	QString& get_display_name();
	/**
	 * Acutal source of media file
	 */
	QUrl get_media_url();

private:
	/**
	 * String to display for humans
	 */
	const QString display_name;
	/**
	 * Acutal source of media file
	 */
	QUrl media_url;

};

class PodCastEpisode : public SeekablePlayableItem
{

public:
	PodCastEpisode();
	virtual ~PodCastEpisode();

	QString get_guid();

private:
	/**
	 * Global Unique ID of item
	 */
	QString guid;
	QDate publication_date;

};

/**
 * Media source with random access in time, player can go back and forth
 */
class SeekablePlayableItem : public PlayableItem
{

public:
	SeekablePlayableItem();
	virtual ~SeekablePlayableItem();

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
	int position_percent;

};

class PodcastEpisode : public SeekablePlayableItem
{

public:
	PodcastEpisode();
	virtual ~PodcastEpisode();

	QString& get_author();
	/**
	 * Synopsis of this episode
	 */
	QString& get_description();
	QString get_guid();

private:
	/**
	 * Author of the episode
	 */
	const QString author;
	/**
	 * Synopsis of this episode
	 */
	const QString description;
	/**
	 * Global Unique ID of item
	 */
	QString guid;
	QDate publication_date;

};
#endif // _PLAYABLEITEM_HPP_ 
