/*******************************************************************************
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
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    roles[BssidRole] = "bssid";
    roles[NameRole] = "name";
    roles[WpsRole] = "wps";
    roles[SignalStrengthRole] = "signal";
    roles[ConnectedRole] = "connected";
    return roles;
}
/******************************************************************************/

int WifiListModel::rowCount(const QModelIndex& /*parent */) const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    WifiControl* instance = WifiControl::get_instance();

    return instance->get_scan_result().size();
}

/******************************************************************************/
QVariant WifiListModel::data(const QModelIndex& index, int role) const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    WifiControl* instance = WifiControl::get_instance();

    auto scan_results = instance->get_scan_result();
    qCDebug(CLASS_LC) << "size: " << scan_results.size();
    if (scan_results.size() <= 0) {
        return QVariant();
    }

    if (index.row() < 0 || index.row() >= scan_results.size())
        return QVariant();

    auto network = scan_results.at(index.row());
     qCDebug(CLASS_LC) << "name: " << network.name << network.signal_strength;
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
}
/******************************************************************************/
