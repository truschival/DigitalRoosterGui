// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GNU PUBLIC LICENSE Version 3 or later
 */

#include <QByteArray>
#include <QHash>
#include <QLoggingCategory>
#include <QQmlEngine>

#include "wifi_control.hpp"
#include "wifilistmodel.hpp"

using namespace DigitalRooster;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.WifiListModel");

/******************************************************************************/
WifiListModel::WifiListModel(QObject* parent)
    : QAbstractListModel(parent) {
}

/******************************************************************************/
QHash<int, QByteArray> WifiListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[BssidRole] = "bssid";
    roles[NameRole] = "name";
    roles[WpsRole] = "wps";
    roles[SignalStrengthRole] = "signal";
    roles[ConnectedRole] = "connected";
    return roles;
}

/******************************************************************************/
int WifiListModel::rowCount(const QModelIndex& /*parent */) const {
    return scan_results.size();
}

/******************************************************************************/
QVariant WifiListModel::data(const QModelIndex& index, int role) const {
    /* static cast only if index.row() is >= 0 and thus can be converted */
    if (index.row() < 0 ||
        static_cast<size_t>(index.row()) >= scan_results.size()) {
        qCCritical(CLASS_LC) << Q_FUNC_INFO << "index out of range " << index;
        return QVariant();
    }
    auto network = scan_results.at(index.row());

    switch (role) {
    case BssidRole:
        return QVariant(network.bssid);
    case NameRole:
        return QVariant(network.name);
    case WpsRole:
        return QVariant(network.wps_available);
    case SignalStrengthRole:
        return QVariant(network.signal_strength);
    case ConnectedRole:
        return QVariant(network.connected);
    }
    return QVariant();
}

/*****************************************************************************/
void WifiListModel::wps_connect(int index) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    /* static cast only if index >= 0 and thus can be converted */
    if (index < 0 || static_cast<size_t>(index) >= scan_results.size()) {
        qCCritical(CLASS_LC) << " index out of bounds";
        return;
    }
    WifiControl::get_instance()->wps_pbc_auth(scan_results.at(index));
}

/******************************************************************************/
void WifiListModel::update_scan_results(
    const std::vector<WifiNetwork>& results) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    beginResetModel();
    scan_results = results;
    endResetModel();
}

/******************************************************************************/
