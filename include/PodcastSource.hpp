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
#include <QObject>
#include <QString>
#include <QVector>
#include <chrono>
#include <limits>
#include <memory>

#include "PlayableItem.hpp"
#include "UpdateTask.hpp"

namespace DigitalRooster {

/**
 * Class to represent a RSS channel with items as episodes
 */
class PodcastSource : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString display_name READ get_title)
    Q_PROPERTY(QUrl url READ get_url)

public:
    /**
     * Preconfigured Podcast Source
     * @param url Feed URL
     */
    explicit PodcastSource(const QUrl& url);

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
    const QString& get_description() const {
        return description;
    }

    /**
     * When was this podcast source last scanned for new items
     */
    const QDateTime& get_last_updated() const {
        return last_updated;
    };

    /**
     * Website of RSS feed channel (not the rss xml URI but additional
     * information)
     */
    const QUrl& get_link() {
        return link;
    }

    /**
     * Logo Image of podcast
     */
    const QUrl& get_image_uri() const {
        return image_uri;
    }
    void set_image_uri(const QUrl& uri) {
        image_uri = uri;
    }

    /**
     * show max_episodes in the list
     */
    int get_max_episodes() {
        return max_episodes;
    };

    /**
     * title element of RSS channel
     */
    const QString& get_title() const {
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
     * will restart the timer if interval is less than current update_interval
     * @param interval
     */
    void set_update_interval(std::chrono::seconds interval);

    /**
     * URL for rss feed of this podcast
     */
    const QUrl& get_url() const {
        return rss_feed_uri;
    }

    /**
     * Description of the Channel (mandatory by RSS2.0 spec)
     */
    void set_description(QString newVal) {
        description = newVal;
        emit descriptionChanged();
    }

    /**
     * When was this podcast source last scanned for new items
     */
    void set_last_updated(QDateTime newVal) {
        last_updated = newVal;
    }

    /**
     * Website of RSS feed channel (not the rss xml URI but additional
     * information)
     */
    void set_link(QUrl newVal) {
        link = newVal;
        emit dataChanged();
    }

    /**
     * set number of displayed/downloaded episodes
     * @param max value >=0
     */
    void set_max_episodes(int max) {
        if (max >= 0)
            max_episodes = max;
    }

    /**
     * title element of RSS channel
     * \param newTitle
     */
    void set_title(QString newTitle) {
        title = newTitle;
        emit titleChanged();
    }

    /**
     * Access to episodes as QList (the Playable items)
     */
    const QVector<std::shared_ptr<PodcastEpisode>>& get_episodes() {
        return episodes;
    }

    /**
     * Access to episode names  as QList (the titles for display in a list)
     */
    QVector<QString> get_episodes_names();

signals:
    /**
     * The episodes list or any other member has been updated
     */
    void dataChanged();

    void titleChanged();

    void descriptionChanged();

	void episodesChanged();

private:
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
     * Map of episodes
     */
    QVector<std::shared_ptr<PodcastEpisode>> episodes;

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
    QUrl image_uri;

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
};
} // namespace DigitalRooster
#endif // _PODCASTSOURCE_HPP_
