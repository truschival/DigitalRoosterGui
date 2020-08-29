// SPDX-License-Identifier: GPL-3.0-or-later
/******************************************************************************
 * \author Thomas Ruschival
 * \copyright 2020 Thomas Ruschival <thomas@ruschival.de>
 * 			  This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			  SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/
#include <QLoggingCategory>
#include <QNetworkInterface>

#include "networkinfo.hpp"
using namespace DigitalRooster;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.NetworkInfo");

/*****************************************************************************/
NetworkInfo::NetworkInfo(QObject* parent)
    : QObject(parent) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}

/*****************************************************************************/
NetworkInfo::NetworkInfo(QString name, QObject* parent)
    : QObject (parent)
    , ifname(name) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}

/*****************************************************************************/
QString NetworkInfo::get_ip_addr() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto itf = QNetworkInterface::interfaceFromName(ifname);
    if (!itf.isValid()) {
        qCCritical(CLASS_LC) << "interface name " << ifname << "not found!";
        return QString();
    }

    auto addr_list = itf.addressEntries();
    if (addr_list.isEmpty()) {
        return QString();
    }
    auto addr = addr_list.first();
    return addr.ip().toString();
}

/*****************************************************************************/
bool NetworkInfo::get_link_status() const {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto itf = QNetworkInterface::interfaceFromName(ifname);
    if (!itf.isValid()) {
        qCCritical(CLASS_LC) << "interface name " << ifname << "not found!";
        return false;
    }
    return (itf.flags() & QNetworkInterface::IsUp);
}

/*****************************************************************************/

