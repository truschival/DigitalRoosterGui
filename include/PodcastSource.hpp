/******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#ifndef _PODCASTSOURCE_HPP_
#define _PODCASTSOURCE_HPP_

#include <QDate>
#include <QDateTime>
#include <QMap>
#include <QDir>
#include <QObject>
#include <QString>
#include <QUuid>
#include <QVector>
#include <chrono>
#include <limits>
#include <memory>

#include "PlayableItem.hpp"
#include "UpdateTask.hpp"


namespace DigitalRooster {
class HttpClient; // forward declaration

/**
 * Class to represent a RSS channel with items as episodes
 */
class PodcastSource : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString display_name READ get_title NOTIFY titleChanged)
    Q_PROPERTY(
        QString description READ get_description NOTIFY descriptionChanged)
    Q_PROPERTY(
        int episode_count READ get_episode_count NOTIFY episodes_count_changed)
    Q_PROPERTY(QUrl url READ get_url)
    Q_PROPERTY(QUrl icon READ get_icon NOTIFY icon_changed)
public:
    /**
     * Preconfigured Podcast Source
     * @param url Feed URL
     * @param cache_dir directory for cache files
     * @param uid unique id for this podcast
     */
    PodcastSource(const QUrl& url, const QDir& cache_dir,
        QUuid uid = QUuid::createUuid());

    /**
     * Destructor to delete icon_downloader nicely
     */
    virtual ~PodcastSource();

    /**
     * Give the Podcast source a (new) update task
     * takes ownership!
     */
    void set_update_task(std::unique_ptr<UpdateTask>&& ut);

    /**
     * Add an episode to episodes
     */
    void add_episode(std::shared_ptr<PodcastEpisode> episode);

    /**
     * Description of the Channel (mandatory by RSS2.0 spec)
     */
    virtual const QString& get_description() const {
        return description;
    }

    /**
     * When was this podcast source last scanned for new items
     */
    virtual const QDateTime& get_last_updated() const {
        return last_updated;
    };

    /**
     * Set cache dir for saving/restoring information
     * @param dirname path to directory (expected to exist)
     */
    void set_cache_dir(const QString& dirname);

    /**
     * cached pocast source information in this file
     * @return file path
     */
    QString get_cache_file_name() const;

    /**
     * find the episode by its id (note: publisher assinged id)
     * @param id episode id (can be UUID or URL format)
     * @return the episode or nullptr
     */
    std::shared_ptr<PodcastEpisode> get_episode_by_id(const QString& id) const;

    /**
     * Website of RSS feed channel (not the rss xml URI but additional
     * information)
     */
    virtual const QUrl& get_link() const {
        return link;
    }

    /**
     * Logo Image URL of podcast
     */
    virtual const QUrl& get_image_url() const {
        return icon_url;
    }
    void set_image_url(const QUrl& uri);

    /**
     * Image file path to local cache of image uri
     * @return local file if it exists
     */
    QString get_image_file_path() const {
        return image_file_path;
    }
    void set_image_file_path(const QString& path);

    /**
     * Return the location of the icon.
     * Either the image_uri if not yet cached or the path to local cache file
     * @return image file path
     */
    QUrl get_icon(){
    	return get_icon_impl();
    }

    /**
     * show max_episodes in the list
     */
    int get_max_episodes() const {
        return max_episodes;
    };

    /**
     * title element of RSS channel
     * @return \ref title
     */
    virtual const QString& get_title() const {
        return title;
    }

    /**
     * Get the PodcastSourceAutoupdating
     * @return interval in ms
     */
    std::chrono::seconds get_update_interval() const {
        return update_interval;
    };

    /**
     * Set the PodcastSourceAutoupdating
     * will restart the timer if interval is less than current \ref
     * update_interval
     * @param interval
     */
    void set_update_interval(std::chrono::seconds interval);

    /**
     * URL for rss feed of this podcast
     * @return \ref rss_feed_uri
     */
    virtual const QUrl& get_url() const {
        return rss_feed_uri;
    }

    /**
     * Update \ref description of the Channel (mandatory by RSS2.0 spec)
     * @param newVal updated description
     */
    void set_description(QString newVal);

    /**
     * When was this podcast source last scanned for new items
     * called by UpdateTask after parsing succeeded.
     */
    void set_last_updated(QDateTime newVal);

    /**
     * Website of RSS feed channel (not the rss xml URI but additional
     * information)
     */
    void set_link(QUrl newVal);

    /**
     * set number of displayed/downloaded episodes
     * @param max value >=0
     */
    void set_max_episodes(int max);

    /**
     * title element of RSS channel
     * \param newTitle
     */
    void set_title(QString newTitle);

    /**
     * Access to \ref episodes (the Playable items)
     */
    const QVector<std::shared_ptr<PodcastEpisode>>& get_episodes() const {
        return get_episodes_impl();
    }

    /**
     * Current number of episodes in list
     * @return number of episodes
     */
    int get_episode_count() const {
        return get_episode_count_impl();
    }

    /**
     * Access to episode names  as QList (the titles for display in a list)
     */
    QVector<QString> get_episodes_names();

    /**
     * unique id for this Podcast RSS source (internally assigned)
     * @return UUID
     */
    QUuid get_id() const {
        return id;
    }

    /**
     * Remove local icon cache file
     */
    void purge_icon_cache();

    /**
     * clear local information on podcast episodes
     */
    void purge_episodes();

public slots:
    /**
     * Triggers immediate update from web
     */
    void refresh();

    /**
     * purges local cache, removes all episodes and updates from internet
     */
    void purge();

    /**
     * save in memory inforamtion to cache file
     */
    void store();

    /**
     * restore information from cache file
     */
    void restore();

    /**
     * Properties of an episode e.g. title etc. has changed and a
     * delayed store should be triggered.
     */
    void episode_info_changed();

signals:
    /**
     * The episodes list or any other member has been updated
     */
    void dataChanged();

    void titleChanged();

    void descriptionChanged();

    void icon_changed();
    /**
     * A new episodes has been added
     * @param count updated number of episodes
     */
    void episodes_count_changed(int count);

private:
    /**
     * unique id for this Podcast RSS source
     */
    const QUuid id;

    /**
     * URL for rss feed of this podcast channel
     */
    QUrl rss_feed_uri;

    /**
     * title element of RSS channel
     */
    QString title;

    /**
     * Description of the Channel (mandatory by RSS2.0 spec)
     */
    QString description;

    /**
     * Container for Episodes of this podcast
     */
    QVector<std::shared_ptr<PodcastEpisode>> episodes;

    /**
     * When was this podcast source last updated (by the publisher)
     */
    QDateTime last_updated;

    /**
     * Timer tor write configuration to disk
     */
    QTimer writeTimer;

    /**
     * Website of RSS feed channel (not the rss xml URI but additional
     * information)
     */
    QUrl link;

    /**
     * Logo Image of podcast
     */
    QUrl icon_url;

    /**
     * Path to local cache of logo pic
     */
    QString image_file_path;

    /**
     * Cache directory
     */
    const QDir& cache_dir;

    /**
     * show max_episodes in the list
     */
    int max_episodes = std::numeric_limits<int>::max();

    /**
     * Interval in ms for auto refresh of content
     * default to 1h
     */
    std::chrono::seconds update_interval{3600LL};

    /**
     * Optional UpdateTask
     */
    std::unique_ptr<UpdateTask> updater = nullptr;

    /**
     * Optional Icon Downloader - only used to refresh icon
     */
    std::unique_ptr<HttpClient> icon_downloader = nullptr;
    /**
     * temporary connection to receive signal form icon_downloader
     */
    QMetaObject::Connection  download_cnx;

    /**
     * implementation of get_cache_file_name() to allow for mocking
     */
    virtual QString get_cache_file_impl() const;

    /**
     * implementation of \ref get_episode_by_id
     * @param id episode id (can be UUID or URL format)
     * @return the episode or nullptr
     */
    virtual std::shared_ptr<PodcastEpisode> get_episode_by_id_impl(
        const QString& id) const;
    /**
     * implementation of \ref get_episodes
     * @return \ref episodes
     */
    virtual const QVector<std::shared_ptr<PodcastEpisode>>&
    get_episodes_impl() const;

    /**
     * Implementation of \ref get_episode_count()
     * return number of episodes
     */
    virtual int get_episode_count_impl() const;

    /**
     * Implementation  of \ref get_icon to get either URL or cached file
     * @return icon url or file path
     */
    virtual QUrl get_icon_impl();

private slots:
	/**
	 * save downloaded byte array in local image file
	 * @param data
	 */
	void store_image(QByteArray data);

};
} // namespace DigitalRooster
#endif // _PODCASTSOURCE_HPP_
