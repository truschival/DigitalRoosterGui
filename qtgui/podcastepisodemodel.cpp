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
#include "PodcastSource.hpp"
#include "mediaplayerproxy.hpp"
#include "podcastepisodemodel.hpp"

using namespace DigitalRooster;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.PodcastEpisodeModel");
/*****************************************************************************/
PodcastEpisodeModel::PodcastEpisodeModel(
    std::shared_ptr<PodcastSource> ps, MediaPlayer& mp, QObject* parent)
    : QAbstractListModel(parent)
    , source(std::move(ps))
    , mpp(mp) {
	qCInfo(CLASS_LC) << Q_FUNC_INFO;
    if (source) {
        // episodes are added or removed when the podcast is updated
        connect(source.get(), &PodcastSource::episodes_count_changed, this,
            [this]() {
                beginResetModel();
                endResetModel();
            });
    }
}

/*****************************************************************************/
const std::vector<std::shared_ptr<PodcastEpisode>>&
PodcastEpisodeModel::episodes() const {
    static const std::vector<std::shared_ptr<PodcastEpisode>> no_episodes;
    return source ? source->get_episodes() : no_episodes;
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
int PodcastEpisodeModel::rowCount(const QModelIndex& /*parent */) const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return episodes().size();
}

/*****************************************************************************/
PodcastEpisode* PodcastEpisodeModel::get_episode(int index) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << index;
    if (index < 0 || static_cast<size_t>(index) >= episodes().size()) {
        qCCritical(CLASS_LC) << Q_FUNC_INFO << "index out of range " << index;
        return nullptr;
    }
    auto ep = episodes()[index].get();
    QQmlEngine::setObjectOwnership(ep, QQmlEngine::CppOwnership);
    return ep;
}

/*****************************************************************************/
void PodcastEpisodeModel::send_to_player(int index) {
	qCDebug(CLASS_LC) << Q_FUNC_INFO << index;
    if (index < 0 || static_cast<size_t>(index) >= episodes().size()) {
        qCCritical(CLASS_LC) << Q_FUNC_INFO << "index out of range " << index;
        return;
    }
    auto ep = episodes()[index];
    mpp.set_media(ep);
    mpp.play();
}

/*****************************************************************************/
QVariant PodcastEpisodeModel::data(const QModelIndex& index, int role) const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << index;
    /* static cast only if index.row() is >= 0 and thus can be converted */
    if (index.row() < 0 ||
        static_cast<size_t>(index.row()) >= episodes().size()) {
        qCCritical(CLASS_LC) << Q_FUNC_INFO << "index out of range " << index;
        return QVariant();
    }

    QString desc;
    auto ep = episodes()[index.row()];
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
