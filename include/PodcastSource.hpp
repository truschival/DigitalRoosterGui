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
#include <QDir>
#include <QJsonObject>
#include <QMap>
#include <QObject>
#include <QString>
#include <QUuid>

#include <chrono>
#include <limits>
#include <memory>
#include <vector>

#include "PlayableItem.hpp"
#include "UpdateTask.hpp"
#include "podcast_serializer.hpp"

namespace DigitalRooster {
class HttpClient; // forward declaration

/**
 * Class represeting information form an RSS Podcast feed with
 * some meta information and \ref DigitalRooster::PodcastEpisode
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
     * @param uid unique id for this podcast
     */
    explicit PodcastSource(QUrl url, QUuid uid = QUuid::createUuid());

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
     * Serializer stores and restores data to/from cache file
     * A PodcastSerializer is responsible for only one PodcastSource,
     * -> PodcastSource takes ownership!
     * @param pser
     */
    void set_serializer(std::unique_ptr<PodcastSerializer>&& pser);

    /**
     * Add an episode to episodes
     */
    void add_episode(const std::shared_ptr<PodcastEpisode>& episode);

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
     * cached podcast source information in this file
     * @return file name (without directory)
     */
    QString get_cache_file_name() const;

    /**
     * Creates a unique file name based on image_url
     * @param image_url url of icon
     * @return uuid_md5(filename(url)).png
     */
    QString create_image_file_name(const QUrl& image_url) const;

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
    QUrl get_icon() {
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
    void set_description(const QString& newVal);

    /**
     * When was this podcast source last scanned for new items
     * called by UpdateTask after parsing succeeded.
     */
    void set_last_updated(const QDateTime& newVal);

    /**
     * Website of RSS feed channel (not the rss xml URI but additional
     * information)
     */
    void set_link(const QUrl& newVal);

    /**
     * set number of displayed/downloaded episodes
     * @param max value >=0
     */
    void set_max_episodes(int max);

    /**
     * title element of RSS channel
     * \param newTitle
     */
    void set_title(const QString& newTitle);

    /**
     * Access to \ref episodes (the Playable items)
     */
    const std::vector<std::shared_ptr<PodcastEpisode>>& get_episodes() const {
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
     * Access to episode names (the titles for display in a list)
     */
    std::vector<QString> get_episodes_names();

    /**
     * unique id for this Podcast RSS source (internally assigned)
     * @return UUID
     */
    QUuid get_id() const {
        return id;
    }

    /**
     * For clients that just want the id without braces
     * @return
     */
    QString get_id_string() const {
        return id.toString(QUuid::WithoutBraces);
    }

    /**
     * Remove local icon cache file
     */
    void purge_icon_cache();

    /**
     * clear local information on podcast episodes
     */
    void purge_episodes();
    /**
     * Create PodcastSource from JSON JSonObject
     * @param json representation
     * @return PodcastSource - default initialized if fields are missing
     */
    static std::shared_ptr<PodcastSource> from_json_object(
        const QJsonObject& json);

    /**
     * Serialize as JSON Object - only contains information that is not
     * updated through RSS feed and cached.
     * @return QJsonObject
     */
    QJsonObject to_json_object() const;

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
     * Properties of an episode e.g. title etc. has changed and a
     * delayed store should be triggered.
     */
    void episode_info_changed();

signals:
    /**
     * Title has changed
     * Signal GUI
     */
    void titleChanged();
    /**
     * Description has been updated
     * Signal GUI
     */

    void descriptionChanged();
    /**
     * Icon image has been updated
     * Signal GUI
     */

    void icon_changed();

    /**
     * The episodes list or any other member has been updated
     * trigger a write
     */
    void dataChanged();

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
    std::vector<std::shared_ptr<PodcastEpisode>> episodes;

    /**
     * When was this podcast source last updated (by the publisher)
     */
    QDateTime last_updated;

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
     * Optional Serializer to cache information on disk
     */
    std::unique_ptr<PodcastSerializer> serializer = nullptr;

    /**
     * Optional Icon Downloader - only used to refresh icon
     */
    std::unique_ptr<HttpClient> icon_downloader = nullptr;
    /**
     * temporary connection to receive signal form icon_downloader
     */
    QMetaObject::Connection download_cnx;

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
    virtual const std::vector<std::shared_ptr<PodcastEpisode>>&
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

    /**
     * Start the download and rescaling of the Podcast image
     */
    virtual void trigger_image_download();
};
} // namespace DigitalRooster
#endif // _PODCASTSOURCE_HPP_
