/******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \author Thomas Ruschival
 * \copyright 2018 Thomas Ruschival <thomas@ruschival.de>
 * 			  This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			  SPDX-License-Identifier: GPL-3.0-or-later
******************************************************************************/
#include <QByteArray>
#include <QDebug>
#include <QHash>
#include <QLoggingCategory>


#include "PodcastSource.hpp"
#include "configuration_manager.hpp"
#include "mediaplayerproxy.hpp"
#include "podcastepisodemodel.hpp"
#include "podcastsourcemodel.hpp"

using namespace DigitalRooster;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.PodcastSourceModel");

/*****************************************************************************/
PodcastSourceModel::PodcastSourceModel(
    std::shared_ptr<ConfigurationManager> confman,
    std::shared_ptr<MediaPlayerProxy> pp, QObject* parent)
    : QAbstractListModel(parent)
    , cm(confman)
    , mpp(pp) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto v = cm->get_podcast_sources();
    for (auto ps : v) {
        connect(
            ps.get(), SIGNAL(titleChanged()), this, SLOT(newDataAvailable()));
    }
}

/*****************************************************************************/

QHash<int, QByteArray> PodcastSourceModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[DisplayNameRole] = "display_name";
    roles[DisplayUrlRole] = "link";
    roles[DisplayCountRole] = "episode_count";
    roles[DescriptionRole] = "description";
    roles[ImageRole] = "logo_image";
    return roles;
}
/*****************************************************************************/

int PodcastSourceModel::rowCount(const QModelIndex& /*parent */) const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return cm->get_podcast_sources().size();
}

/*****************************************************************************/
void PodcastSourceModel::newDataAvailable() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    emit dataChanged(createIndex(0, 0), createIndex(rowCount() - 1, 0));
}

/*****************************************************************************/
PodcastEpisodeModel* PodcastSourceModel::get_episodes(int index) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    auto v = cm->get_podcast_sources();
    if (index < 0 || index >= v.size())
        return nullptr;

    return new PodcastEpisodeModel(&(v[index]->get_episodes()), mpp, this);
}

/*****************************************************************************/
QVariant PodcastSourceModel::data(const QModelIndex& index, int role) const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto v = cm->get_podcast_sources();
    if (index.row() < 0 || index.row() >= v.size())
        return QVariant();
    QString desc;
    auto ps = v[index.row()];

    switch (role) {
    case DisplayNameRole:
        return QVariant(ps->get_title());
    case DisplayUrlRole:
        return ps->get_url();
    case DisplayCountRole:
        return ps->get_episodes().size();
    case DescriptionRole:
    	desc = ps->get_description();
    	desc.remove(QRegExp("<[^>]*>")); //Strip HTML tags
        return QVariant(desc);
    case ImageRole:
        return ps->get_icon();
    }
    return QVariant();
}

/*****************************************************************************/
void PodcastSourceModel::refresh(int index) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    try {
        cm->get_podcast_source_by_index(index)->refresh();
    } catch (std::out_of_range&) {
        qCCritical(CLASS_LC) << "index out of range " << index;
    }
}

/*****************************************************************************/
void PodcastSourceModel::purge(int index) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    try {
        cm->get_podcast_source_by_index(index)->purge();
    } catch (std::out_of_range&) {
        qCCritical(CLASS_LC) << "index out of range " << index;
    }
}

/*****************************************************************************/
void PodcastSourceModel::remove(int index) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    beginRemoveRows(QModelIndex(), index, index);
    try {
        cm->remove_podcast_source_by_index(index);
    } catch (std::out_of_range&) {
        qCCritical(CLASS_LC) << "index out of range " << index;
    }
    endRemoveRows();
}
