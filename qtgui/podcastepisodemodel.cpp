/*************************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \author Thomas Ruschival
 * \copyright 2018 Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 2 or later
 *
 * 			 SPDX-License-Identifier: GPL-2.0-or-later}
 *************************************************************************************/
#include <QByteArray>
#include <QHash>
#include <QDebug>

#include "PlayableItem.hpp"
#include "podcastepisodemodel.hpp"
using namespace DigitalRooster;

/*************************************************************************************/
PodcastEpisodeModel::PodcastEpisodeModel(
    const QVector<std::shared_ptr<PodcastEpisode>>* ep, QObject* parent)
    : QAbstractListModel(parent)
    , episodes(ep) {
}

/*************************************************************************************/
PodcastEpisodeModel::PodcastEpisodeModel(QObject* parent)
    : QAbstractListModel(parent)
    , episodes(nullptr) {
}

/*************************************************************************************/

QHash<int, QByteArray> PodcastEpisodeModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[DisplayNameRole] = "display_name";
    roles[LengthRole] = "length";
    roles[CurrentPositionRole] = "current_position";
    roles[DescriptionRole] = "description";
    roles[DateRole] = "pub_date";
    return roles;
}
/*************************************************************************************/
void PodcastEpisodeModel::set_episodes(
    const QVector<std::shared_ptr<PodcastEpisode>>* ep) {
    episodes = ep;
}

/*************************************************************************************/

int PodcastEpisodeModel::rowCount(const QModelIndex& /*parent */) const {
	qDebug() << __FUNCTION__;
    if (!episodes){
    	qWarning() << " no episodes ";
        return 0;
    }
    return episodes->size();
}


/*************************************************************************************/
QVariant PodcastEpisodeModel::data(const QModelIndex& index, int role) const {
	qDebug() << __FUNCTION__ << "(" << index.row() << ")";
	if (!episodes)
	        return QVariant();

	if (index.row() < 0 || index.row() >= episodes->size())
	        return QVariant();

	    auto ep = episodes->at(index.row());

	    switch (role) {
	    case DisplayNameRole:
	        return QVariant(ep->get_display_name());
	    case LengthRole:
	        return QVariant(ep->get_length());
	    case CurrentPositionRole:
	        return QVariant(ep->get_position());
	    case DescriptionRole:
	        return QVariant(ep->get_description());
	    case DateRole:
	        return QVariant(ep->get_publication_date());
	    }

    return QVariant();
}
