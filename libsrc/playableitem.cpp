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

#include <QDebug>
#include <QLoggingCategory>
#include <QUrl>
#include <QUuid>

#include "PlayableItem.hpp"

using namespace DigitalRooster;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.PlayableItem");

/***********************************************************************/
PlayableItem::PlayableItem(const QUuid& uid)
    : id(uid){};

/***********************************************************************/

PlayableItem::PlayableItem(
    const QString& name, const QUrl& url, const QUuid& uid)
    : id(uid)
    , display_name(name)
    , media_url(url){};

/***********************************************************************/
void PlayableItem::set_position(qint64 newVal) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    if (newVal >= 0) {
        position = newVal;
        emit position_updated(newVal);
        emit data_changed();
    }
};

/***********************************************************************/
void PlayableItem::set_url(const QUrl& url) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    media_url = url;
};

/***********************************************************************/

void PlayableItem::set_title(const QString& title) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    this->title = title;
    emit display_name_changed(do_get_display_name());
    emit data_changed();
};

/***********************************************************************/
void PlayableItem::set_publisher(const QString& publisher) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    this->publisher = publisher;
    emit display_name_changed(do_get_display_name());
    emit data_changed();
};


/***********************************************************************/
void PlayableItem::set_display_name(const QString& name) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    display_name = name;
}

/***********************************************************************/
QString PlayableItem::do_get_display_name() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return display_name;
}

/***********************************************************************/
QString PodcastEpisode::do_get_display_name() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto publisher = get_publisher();
    auto title = get_title();

    if (title.isEmpty() && publisher.isEmpty()) {
        // default to generic human config assigned name
        return PlayableItem::do_get_display_name();
    }
    if (!title.isEmpty() && publisher.isEmpty()) {
        return title;
    }
    if (title.isEmpty() && !publisher.isEmpty()) {
        return publisher;
    }
    return publisher + ": " + title;
}

/***********************************************************************/
PodcastEpisode::PodcastEpisode(const QString& title, const QUrl& url)
    : PlayableItem(title, url)
    , publication_date(QDateTime::currentDateTime()) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    set_title(title);
}

/***********************************************************************/
const QString& PodcastEpisode::get_description() const {
    return description;
}

/***********************************************************************/
void PodcastEpisode::set_description(const QString& desc) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    description = desc;
    emit description_changed(desc);
    emit data_changed();
}

/***********************************************************************/
QString PodcastEpisode::get_guid() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (guid.isEmpty()) {
        return get_url().toString();
    }
    return guid;
}

/***********************************************************************/
void PodcastEpisode::set_guid(const QString& uid) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    guid = uid;
}

/***********************************************************************/
const QDateTime& PodcastEpisode::get_publication_date() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    return publication_date;
}

/***********************************************************************/
void PodcastEpisode::set_duration(qint64 len) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (len >= 0) {
        duration = len;
        emit duration_changed(len);
        emit data_changed();
    }
}
/***********************************************************************/
void PodcastEpisode::set_position(qint64 newVal) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    // let base class check pre-conditions and set value (is private)
    PlayableItem::set_position(newVal);

    auto d = get_duration();
    if (d != 0 && !listened) {
        // current positon is at least 10%
        auto perc = (10 * get_position()) / d;
        if (perc >= 1) {
            listened = true;
            emit listened_changed(true);
        };
    }
}

/***********************************************************************/
bool PodcastEpisode::already_listened() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return listened; // no duration, always not listened
}

/***********************************************************************/
void PodcastEpisode::set_publication_date(const QDateTime& date) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    if (!date.isValid()) {
        qCCritical(CLASS_LC) << "invalid QDateTime!";
        return;
    }
    publication_date = date;
    emit publication_date_changed(date);
    emit data_changed();
}

/***********************************************************************/
