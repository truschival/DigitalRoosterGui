// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

#include <QByteArray>
#include <QDebug>
#include <QHash>
#include <QLoggingCategory>
#include <QQmlEngine>
#include <QRegExp>

#include "PlayableItem.hpp"
#include "mediaplayerproxy.hpp"
#include "podcastepisodemodel.hpp"

using namespace DigitalRooster;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.PodcastEpisodeModel");
/*****************************************************************************/
PodcastEpisodeModel::PodcastEpisodeModel(
    const std::vector<std::shared_ptr<PodcastEpisode>>* ep, MediaPlayer& mp,
    QObject* parent)
    : QAbstractListModel(parent)
    , episodes(ep)
    , mpp(mp) {
	qCInfo(CLASS_LC) << Q_FUNC_INFO;
}

/*****************************************************************************/
PodcastEpisodeModel::~PodcastEpisodeModel(){
	qCInfo(CLASS_LC) << Q_FUNC_INFO;
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
    const std::vector<std::shared_ptr<PodcastEpisode>>* ep) {
    episodes = ep;
}

/*****************************************************************************/
int PodcastEpisodeModel::rowCount(const QModelIndex& /*parent */) const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (!episodes) {
    	qCWarning(CLASS_LC) << " no episodes ";
        return 0;
    }
    return episodes->size();
}

/*****************************************************************************/
PodcastEpisode* PodcastEpisodeModel::get_episode(int index) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << index;
    auto ep = episodes->at(index).get();
    QQmlEngine::setObjectOwnership(ep, QQmlEngine::CppOwnership);
    return ep;
}

/*****************************************************************************/
void PodcastEpisodeModel::send_to_player(int index) {
	qCDebug(CLASS_LC) << Q_FUNC_INFO << index;
	auto ep = episodes->at(index);
    mpp.set_media(ep);
    mpp.play();
}

/*****************************************************************************/
QVariant PodcastEpisodeModel::data(const QModelIndex& index, int role) const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << index;
    if (!episodes)
        return QVariant();

    /* static cast only if index.row() is >= 0 and thus can be converted */
    if (index.row() < 0 ||
        static_cast<size_t>(index.row()) >= episodes->size()) {
        qCCritical(CLASS_LC) << Q_FUNC_INFO << "index out of range " << index;
        return QVariant();
    }

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
        desc.remove(QRegExp("<[^>]*>")); // Strip HTML tags
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
