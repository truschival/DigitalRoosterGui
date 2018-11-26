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

    if (newVal <= duration && newVal >= 0) {
        position = newVal;
    }
};

/***********************************************************************/
void PlayableItem::set_duration(qint64 len) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (len >= 0) {
        duration = len;
    }
}

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
};

/***********************************************************************/
void PlayableItem::set_publisher(const QString& publisher) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    this->publisher = publisher;
    emit display_name_changed(do_get_display_name());
};

/***********************************************************************/
QString PlayableItem::do_get_display_name() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (title.isEmpty() && publisher.isEmpty()) {
        return display_name;
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
