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

#ifndef _PODCASTSOURCE_HPP_
#define _PODCASTSOURCE_HPP_

#include <QDate>
#include <QFile>
#include <QMap>
#include <QObject>
#include <QString>
#include <cstddef> //size_t
#include <limits>
#include <memory>

#include "PlayableItem.hpp"

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
    explicit PodcastSource(QUrl url)
        : rss_feed_uri(url){};

    /**
     * Preconfigured Podcast Source
     * @param url Feed URL
     */
    explicit PodcastSource(const char* url)
        : rss_feed_uri(url){};

    /**
     * Add an episode to episodes
     */
    void add_episode(std::shared_ptr<PodcastEpisode> episode);

    /**
     * Deletes all episodes from list
     */
    void flush_episodes();

    /**
     * Description of the Channel (mandatory by RSS2.0 spec)
     */
    const QString& get_description() {
        return description;
    }

    /**
     * When was this podcast source last scanned for new items
     */
    const QDateTime& get_last_updated() {
        return last_updated;
    };

    /**
     * Website of RSS feed channel (not the rss xml URI but additional information)
     */
    const QUrl& get_link() {
        return link;
    }

    /**
     * show max_episodes in the list
     */
    size_t get_max_episodes() {
        return max_episodes;
    };

    /**
     * path local XML file for RSS feed
     */
    const QString& get_rss_file() {
        return rss_file;
    }

    /**
     * title element of RSS channel
     */
    const QString& get_title() {
        return title;
    }

    /**
     * seconds until update is meaningful. Calculated from optional items skip_days
     * and skip_hours
     */
    int get_update_period() {
        return update_period;
    };
    /**
     * URL for rss feed of this podcast
     */
    const QUrl& get_url() {
        return rss_feed_uri;
    }

    /**
     * Remove an episode from the list of episodes
     * @param name episode to remove
     */
    void remove_episode(const QString& name);

    /**
     * Description of the Channel (mandatory by RSS2.0 spec)
     */
    void set_description(QString newVal) {
        description = newVal;
    }

    /**
     * When was this podcast source last scanned for new items
     */
    void set_last_updated(QDateTime newVal) {
        last_updated = newVal;
    }

    /**
     * Website of RSS feed channel (not the rss xml URI but additional information)
     */
    void set_link(QUrl newVal) {
        link = newVal;
    }

    /**
     * set number of displayed/downloaded episodes
     */
    void set_max_episodes(size_t max) {
        max_episodes = max;
    }

    /**
     * local XML file last read from internet
     */
    void set_rss_file(QString filepath) {
        rss_file = filepath;
    }

    /**
     * title element of RSS channel
     * \param newTitle
     */
    void set_title(QString newTitle) {
        title = newTitle;
    }

    /**
     * seconds until update is meaningful. Calculated from optional items skip_days
     * and skip_hours
     * \param period new period
     */
    void set_update_period(int period) {
        update_period = period;
    }

    /**
     * Access to episodes as QList (the Playable items)
     */
    QList<std::shared_ptr<PodcastEpisode>> get_episodes() {
        return episodes.values();
    }

    /**
     * Access to episode names  as QList (the titles for display in a list)
     */
    QList<QString> get_episodes_names() {
        return episodes.keys();
    }
public slots:
    /**
     * Updated Feed received
     * @param filename on disk
     */
    void newFileAvailable(const QString& filename);

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
    QMap<QString, std::shared_ptr<PodcastEpisode>> episodes;

    /**
     * When was this podcast source last updated (by the publisher)
     */
    QDateTime last_updated;

    /**
     * Website of RSS feed channel (not the rss xml URI but additional information)
     */
    QUrl link;
    /**
     * show max_episodes in the list
     */
    size_t max_episodes = std::numeric_limits<size_t>::max();
    /**
     * path local XML file for RSS feed
     */
    QString rss_file;

    /**
     * seconds until update is meaningful. Calculated from optional items skip_days
     * and skip_hours
     */
    int update_period = 0;
};
}
#endif // _PODCASTSOURCE_HPP_
