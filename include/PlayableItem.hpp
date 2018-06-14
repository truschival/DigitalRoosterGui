/******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 2 or later
 * 			 SPDX-License-Identifier: GPL-2.0-or-later}
 *
 *****************************************************************************/

#ifndef _PLAYABLEITEM_HPP_
#define _PLAYABLEITEM_HPP_

#include <QDateTime>
#include <QDebug>
#include <QObject>
#include <QString>
#include <QUrl>

namespace DigitalRooster {
/**
 * Interface of Playable Item : self contained information to play
 */
class PlayableItem : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString display_name READ get_display_name WRITE set_display_name)
    Q_PROPERTY(QUrl url READ get_url WRITE set_url)
	Q_PROPERTY(int position READ get_position WRITE set_position)
	Q_PROPERTY(int duration READ get_duration WRITE set_duration)
public:
    /**
     * Default Constructor
     */
    PlayableItem() = default;

    /**
     * Convenience Constructor
     * @param name display_name
     * @param url media_url
     */
    PlayableItem(const QString& name, const QUrl& url)
        : display_name(name)
        , media_url(url){};

    const QString& get_display_name() const{
        return display_name;
    };

    const QUrl& get_url() const {
        return media_url;
    };

    void set_display_name(const QString& name) {
        display_name = name;
    };


    void set_url(const QUrl& url) {
        media_url = url;
    };

	/**
	* last stored position
	*/
	qint64 get_position() const {
		return position;
	};
	/**
	* update position of already listened content
	* @param newVal current position in stream
	*/
	void set_position(qint64 newVal) {
		if (newVal <= duration && newVal >= 0) {
			position = newVal;
		}
	};


	/**
	* Total length of media in ms
	* @return
	*/
	qint64 get_duration() {
		return duration;
	}

	/**
	* Update length in ms only used for display purpose
	* @param len >= 0
	*/
	void set_duration(qint64 len) {
		if (len >= 0)
			duration = len;
	}

private:
    /** human readable name*/
    QString display_name;

    /** Media URL */
    QUrl media_url;

	/**
	* Current Position in stream
	*/
	qint64 position = 0;

	/**
	* Total length in ms
	*/
	qint64 duration = 0;
};

/**
 * PodcastEpisode = item of RSS feed
 */
class PodcastEpisode : public PlayableItem {
	Q_OBJECT
	Q_PROPERTY(QString author READ get_author)
	Q_PROPERTY(QString description READ get_description)
public:
    PodcastEpisode() = default;

    /**
     * Convenience constructor
     * @param name
     * @param url
     */
    PodcastEpisode(const QString& name, const QUrl& url)
        : PlayableItem(name, url){

          };
    virtual ~PodcastEpisode() = default;

    void set_author(const QString& newAuthor) {
        author = newAuthor;
    };

    const QString& get_author() const{
        return author;
    }

    const QString& get_description() const{
        return description;
    }

    void set_description(const QString& desc) {
        description = desc;
    };

    QString get_guid() const{
        if (guid.isEmpty()) {
            return get_url().toString();
        }
        return guid;
    }

    void set_guid(const QString& uid) {
        guid = uid;
    };

    const QDateTime& get_publication_date() const {
        return publication_date;
    }

    void set_publication_date(const QDateTime& date) {
        if (date.isValid())
            publication_date = date;
    };

private:
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
    QDateTime publication_date;
};
};
#endif // _PLAYABLEITEM_HPP_
