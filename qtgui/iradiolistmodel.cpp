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

#include "PlayableItem.hpp"
#include "iradiolistmodel.hpp"
#include "mediaplayerproxy.hpp"

using namespace DigitalRooster;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.IRadioListModel");
/*****************************************************************************/
IRadioListModel::IRadioListModel(
    IStationStore& store, MediaPlayer& mp, QObject* parent)
    : QAbstractListModel(parent)
    , config(store)
    , mpp(mp) {
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
    auto sz = config.get_stations().size();
    return sz;
}

/*****************************************************************************/
QUrl IRadioListModel::get_station_url(int index) {
    auto pi = config.get_stations().at(index);
    return pi->get_url();
}

/*****************************************************************************/
void IRadioListModel::send_to_player(int index) {
    auto station = config.get_stations().at(index);
    mpp.set_media(station);
    mpp.play();
}

/*****************************************************************************/
QVariant IRadioListModel::data(const QModelIndex& index, int role) const {
    auto sz = config.get_stations().size();

    /* static cast only if index.row() is >= 0 and thus can be converted */
    if (index.row() < 0 || static_cast<size_t>(index.row()) >= sz) {
        qCCritical(CLASS_LC) << Q_FUNC_INFO << "index out of range " << index;
        return QVariant();
    }

    auto station = config.get_stations().at(index.row());

    switch (role) {
    case StationNameRole:
        return QVariant(station->get_display_name());
    case UriRole:
        return QVariant(station->get_url());
    }

    return QVariant();
}
