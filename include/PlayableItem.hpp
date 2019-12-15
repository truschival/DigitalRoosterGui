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

#ifndef _PLAYABLEITEM_HPP_
#define _PLAYABLEITEM_HPP_

#include <QDateTime>
#include <QDebug>
#include <QObject>
#include <QString>
#include <QUrl>
#include <QUuid>

namespace DigitalRooster {

/**
 * Interface of Playable Item : self contained information to play
 */
class PlayableItem : public QObject {
    Q_OBJECT
    Q_PROPERTY(
        QString display_name READ get_display_name NOTIFY display_name_changed)
    Q_PROPERTY(QString title READ get_title WRITE set_title NOTIFY
            display_name_changed)
    Q_PROPERTY(QString publisher READ get_publisher WRITE set_publisher NOTIFY
            display_name_changed)
    Q_PROPERTY(QUrl url READ get_url WRITE set_url)
    Q_PROPERTY(QUuid id READ get_id)
    Q_PROPERTY(int position READ get_position WRITE set_position)
public:
    /**
     * Default Constructor
     */
    explicit PlayableItem(const QUuid& uid = QUuid::createUuid());

    /**
     * Convenience Constructor
     * @param name display_name
     * @param url  media_url
     * @param uid  (optional) unique id
     */
    PlayableItem(const QString& name, const QUrl& url,
        const QUuid& uid = QUuid::createUuid());

    /**
     * Information on title, publisher ,etc.
     * @return  display_name
     */
    QString get_display_name() const {
        return do_get_display_name();
    };
    /**
     * Update (station-) name
     * @param name \ref display_name
     */
    void set_display_name(const QString& name);

    /**
     * Stream source URI
     * @return media uri
     */
    const QUrl& get_url() const {
        return media_url;
    };

    void set_url(const QUrl& url);

    /**
     * last stored position
     */
    qint64 get_position() const {
        return position;
    };
    /**
     * update \ref position - emits \ref position_updated
     * @param newVal current position in stream
     */
    virtual void set_position(qint64 newVal);

    /**
     * Mark as seekable: position can be updated
     */
    void set_seekable(bool seek){
    	seekable = seek;
    }
    bool is_seekable() const{
    	return seekable;
    };

    /**
     * Title for Playable item
     * @return title
     */
    QString get_title() const {
        return title;
    };
    void set_title(const QString& title);

    /**
     * Publisher/Radio station of this playable item
     * @return publisher
     */
    QString get_publisher() const {
        return publisher;
    };
    void set_publisher(const QString& publisher);

    /**
     * unique id for this Item
     * @return
     */
    QUuid get_id() const {
        return id;
    }

signals:
    /**
     * Human readable identifier changed (title, name, publisher etc)
     * \param info new \ref display_name
     */
    void display_name_changed(const QString& info);

    /**
     * Position has changed
     * \param newpos updated position
     */
    void position_updated(qint64 newpos);

    /**
     * Any information changed
     */
    void data_changed();

private:
    /**
     * 'unique' id for this object, info managed by configuration_manager
     */
    const QUuid id;

    /** initial human assigned display name*/
    QString display_name;

    /** Title */
    QString title;

    /** publisher/author/station name */
    QString publisher;

    /** Media URL */
    QUrl media_url;

    /** Current position in stream */
    qint64 position = 0;

    /** Can ressource positon be set ?*/
    bool seekable{false};

protected:
    /**
     * Display name means differnt things to differnt playable items
     * here \ref display_name ....
     * @return string representation \ref display_name
     */
    virtual QString do_get_display_name() const;
};

/**
 * PodcastEpisode = item of a RSS feed
 */
class PodcastEpisode : public PlayableItem {
    Q_OBJECT
    Q_PROPERTY(
        QString description READ get_description NOTIFY description_changed)
    Q_PROPERTY(bool listened READ already_listened NOTIFY listened_changed)
    Q_PROPERTY(int duration READ get_duration WRITE set_duration NOTIFY
            duration_changed)
public:
    PodcastEpisode() = default;

    /**
     * Convenience constructor
     * @param title
     * @param url
     */
    PodcastEpisode(const QString& title, const QUrl& url);

    virtual ~PodcastEpisode() = default;

    /**
     * Human readable description written by Episode publisher assinged during
     * RSS parsing
     * @param desc description
     */
    void set_description(const QString& desc);
    const QString& get_description() const;

    /**
     * Podcast publisher assinged unique ID can be URL or UUID formatted string
     * @param uid new unique ID
     */
    void set_guid(const QString& uid);
    QString get_guid() const;

    /**
     * Publication date of podcast episode
     * @param date date&time
     */
    void set_publication_date(const QDateTime& date);
    const QDateTime& get_publication_date() const;

    /**
     * update \ref position
     *   emits \ref position_updated
     *   emits \ref listened_changed(bool)
     * @param newVal current position in stream
     */
    virtual void set_position(qint64 newVal) override;

    /**
     * Check if podcast episode is considered listened
     */
    bool already_listened() const;

    /**
     * Total length of media in ms
     * @return
     */
    qint64 get_duration() const {
        return duration;
    }

    /**
     * Update length in ms only used for display purpose
     * @param len >= 0
     */
    void set_duration(qint64 len);

signals:
    void description_changed(const QString& desc);

    void publication_date_changed(const QDateTime& datetime);

    /**
     * duration has been updated - does this happen?
     */
    void duration_changed(qint64 duration);
    /**
     * The podcast episoded is considered listend/new
     */
    void listened_changed(bool listened);

private:
    /**
     * Synopsis of this episode
     */
    QString description;

    /**
     * Global Unique ID of podcast, assigned by publisher in RSS
     * Do not confuse with PlayableItem.id
     */
    QString guid;

    /**
     * Release date of episode (item)
     */
    QDateTime publication_date;

    /**
     * Total duration in ms
     */
    qint64 duration = 0;

    /**
     * local flag to check if listened status changed
     */
    bool listened = false;

protected:
    /**
     * Create human information dynamically from combination of
     * \ref title, \ref publisher \ref display_name ....
     * @return string representation
     */
    virtual QString do_get_display_name() const override;
};
};     // namespace DigitalRooster
#endif // _PLAYABLEITEM_HPP_
