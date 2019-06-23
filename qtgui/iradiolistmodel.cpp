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
 *****************************************************************************/

#include <QByteArray>
#include <QDebug>
#include <QHash>
#include <QQmlEngine>

#include "PlayableItem.hpp"
#include "configuration_manager.hpp"
#include "iradiolistmodel.hpp"
#include "mediaplayerproxy.hpp"

using namespace DigitalRooster;

/*****************************************************************************/
IRadioListModel::IRadioListModel(
    std::shared_ptr<DigitalRooster::ConfigurationManager> confman,
    std::shared_ptr<DigitalRooster::MediaPlayerProxy> pp, QObject* parent)
    : QAbstractListModel(parent)
    , cm(confman)
    , mpp(pp) {
}


/*****************************************************************************/
IRadioListModel::IRadioListModel(QObject* parent)
    : QAbstractListModel(parent)
    , cm(nullptr) {
}

/*****************************************************************************/
QHash<int, QByteArray> IRadioListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[StationNameRole] = "station_name";
    roles[UriRole] = "uri";
    return roles;
}

/*****************************************************************************/
int IRadioListModel::rowCount(const QModelIndex& /*parent */) const {
    // qDebug() << __FUNCTION__;
    if (cm->get_stream_sources().size() <= 0) {
        qWarning() << " no stations ";
    }
    return cm->get_stream_sources().size();
}

/*****************************************************************************/
QUrl IRadioListModel::get_station_url(int index) {
    auto pi = cm->get_stream_sources().at(index);
    return pi->get_url();
}

/*****************************************************************************/
void IRadioListModel::send_to_player(int index) {
    auto station = cm->get_stream_sources().at(index);
    mpp->set_media(station);
    mpp->play();
}

/*****************************************************************************/
QVariant IRadioListModel::data(const QModelIndex& index, int role) const {
    // qDebug() << __FUNCTION__ << "(" << index.row() << ")";
    if (cm->get_stream_sources().size() <= 0)
        return QVariant();

    if (index.row() < 0 || index.row() >= cm->get_stream_sources().size())
        return QVariant();

    auto station = cm->get_stream_sources().at(index.row());

    switch (role) {
    case StationNameRole:
        return QVariant(station->get_display_name());
    case UriRole:
        return QVariant(station->get_url());
    }

    return QVariant();
}
