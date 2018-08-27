/*************************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \author Thomas Ruschival
 * \copyright 2018 Thomas Ruschival <thomas@ruschival.de>
 * 			  This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			  SPDX-License-Identifier: GPL-3.0-or-later
 *************************************************************************************/
#include <QByteArray>
#include <QHash>

#include "PodcastSource.hpp"
#include "configuration_manager.hpp"
#include "mediaplayerproxy.hpp"
#include "podcastepisodemodel.hpp"
#include "podcastsourcemodel.hpp"

using namespace DigitalRooster;
/*************************************************************************************/

PodcastSourceModel::PodcastSourceModel(
    std::shared_ptr<ConfigurationManager> confman,
    std::shared_ptr<MediaPlayerProxy> pp, QObject* parent)
    : QAbstractListModel(parent)
    , cm(confman)
    , mpp(pp) {
    auto v = cm->get_podcast_sources();
    for (auto ps : v) {
        connect(ps.get(), SIGNAL(dataChanged()), this,
            SLOT(dataChanged()));
    }
}
/*************************************************************************************/

QHash<int, QByteArray> PodcastSourceModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[DisplayNameRole] = "display_name";
    roles[DisplayUrlRole] = "link";
    roles[DisplayCountRole] = "episode_count";
    roles[DescriptionRole] = "description";
    roles[ImageRole] = "logo_image";
    return roles;
}
/*************************************************************************************/

int PodcastSourceModel::rowCount(const QModelIndex& /*parent */) const {
    return cm->get_podcast_sources().size();
}

/*************************************************************************************/

void PodcastSourceModel::newDataAvailable() {
    emit dataChanged(createIndex(0, 0), createIndex(rowCount() - 1, 0));
}

/*******************************************************************************/
PodcastEpisodeModel* PodcastSourceModel::get_episodes(int index) {
    // qDebug() << __FUNCTION__ << " index: " << index;

    auto v = cm->get_podcast_sources();
    if (index < 0 || index >= v.size())
        return nullptr;

    return new PodcastEpisodeModel(&(v[index]->get_episodes()), mpp);
}
/*************************************************************************************/

QVariant PodcastSourceModel::data(const QModelIndex& index, int role) const {
    auto v = cm->get_podcast_sources();
    if (index.row() < 0 || index.row() >= v.size())
        return QVariant();

    auto ps = v[index.row()];

    switch (role) {
    case DisplayNameRole:
        return QVariant(ps->get_title());
    case DisplayUrlRole:
        return ps->get_url();
    case DisplayCountRole:
        return ps->get_episodes().size();
    case DescriptionRole:
        return ps->get_description();
    case ImageRole:
        return ps->get_image_uri();
    }
    return QVariant();
}
