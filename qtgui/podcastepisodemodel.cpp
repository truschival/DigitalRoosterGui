/*****************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \author Thomas Ruschival
 * \copyright 2018 Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 *
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 ******************************************************************************/
#include <QByteArray>
#include <QRegExp>
#include <QDebug>
#include <QHash>
#include <QQmlEngine>

#include "PlayableItem.hpp"
#include "mediaplayerproxy.hpp"
#include "podcastepisodemodel.hpp"

using namespace DigitalRooster;

/*****************************************************************************/
PodcastEpisodeModel::PodcastEpisodeModel(
    const QVector<std::shared_ptr<PodcastEpisode>>* ep,
    MediaPlayer& mp, QObject* parent)
    : QAbstractListModel(parent)
    , episodes(ep)
    , mpp(mp) {
}

/*****************************************************************************/

QHash<int, QByteArray> PodcastEpisodeModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[DisplayNameRole] = "display_name";
    roles[TitleRole] = "title";
    roles[PublisherRole] = "publisher";
    roles[DurationRole] = "duration";
    roles[CurrentPositionRole] = "current_position";
    roles[DescriptionRole] = "description";
    roles[DateRole] = "pub_date";
    roles[ListenedRole] = "listened";
    return roles;
}
/*****************************************************************************/
void PodcastEpisodeModel::set_episodes(
    const QVector<std::shared_ptr<PodcastEpisode>>* ep) {
    episodes = ep;
}

/*****************************************************************************/

int PodcastEpisodeModel::rowCount(const QModelIndex& /*parent */) const {
    // qDebug() << __FUNCTION__;
    if (!episodes) {
        qWarning() << " no episodes ";
        return 0;
    }
    return episodes->size();
}

/*****************************************************************************/
PodcastEpisode* PodcastEpisodeModel::get_episode(int index) {
    // qDebug() << __FUNCTION__ << " index: " << index;
    auto ep = episodes->at(index).get();
    QQmlEngine::setObjectOwnership(ep, QQmlEngine::CppOwnership);
    return ep;
}

/*****************************************************************************/
void PodcastEpisodeModel::send_to_player(int index) {
    auto ep = episodes->at(index);
    mpp.set_media(ep);
    mpp.play();
}

/*****************************************************************************/
QVariant PodcastEpisodeModel::data(const QModelIndex& index, int role) const {
    // qDebug() << __FUNCTION__ << "(" << index.row() << ")";
    if (!episodes)
        return QVariant();

    if (index.row() < 0 || index.row() >= episodes->size())
        return QVariant();

    QString desc;
    auto ep = episodes->at(index.row());
    auto duration = QTime::fromMSecsSinceStartOfDay(ep->get_duration());

    switch (role) {
    case DisplayNameRole:
        return QVariant(ep->get_display_name());
    case TitleRole:
        return QVariant(ep->get_title());
    case PublisherRole:
        return QVariant(ep->get_publisher());
    case DurationRole:
        return QVariant(duration.toString("hh:mm:ss"));
    case CurrentPositionRole:
        return QVariant(ep->get_position());
    case DescriptionRole:
    	desc = ep->get_description();
    	desc.remove(QRegExp("<[^>]*>")); //Strip HTML tags
        return QVariant(desc);
    case ListenedRole:
        return QVariant(ep->already_listened());
    case DateRole:
        auto date = ep->get_publication_date();
        if (date.isValid()) {
            return QVariant(date.toString("dd.MMM.yyyy"));
        }
    }

    return QVariant();
}
