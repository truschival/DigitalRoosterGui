// SPDX-License-Identifier: GPL-3.0-or-later
/******************************************************************************
 * \author Thomas Ruschival
 * \copyright 2020 Thomas Ruschival <thomas@ruschival.de>
 * 			  This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			  SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/
#include <QLoggingCategory>
#include <QNetworkInterface>
#include <QTimerEvent>
#include <chrono>

#include "networkinfo.hpp"
using namespace DigitalRooster;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.NetworkInfo");

/*****************************************************************************/
NetworkInfo::NetworkInfo(QString name, QObject* parent)
    : QObject(parent)
    , ifname(name) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    update_net_info();
    // Event loop timer check every second if interface status changed
    evt_timer_id = startTimer(std::chrono::seconds(1));
}

/*****************************************************************************/
void NetworkInfo::timerEvent(QTimerEvent* evt) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (evt->timerId() == evt_timer_id) {
        update_net_info();
    } else {
        QObject::timerEvent(evt);
    }
}

/*****************************************************************************/
void NetworkInfo::update_net_info() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto itf = QNetworkInterface::interfaceFromName(ifname);
    if (!itf.isValid()) {
        qCCritical(CLASS_LC) << "interface name " << ifname << "not found!";
        emit link_status_changed(false);
        return;
    }
    bool cur_state = (itf.flags() & QNetworkInterface::IsUp);
    if (cur_state != if_state) {
        qCDebug(CLASS_LC) << "Link changed to" << cur_state;
        if_state = cur_state;
        emit link_status_changed(if_state);
    }

    auto addr_list = itf.addressEntries();
    QString addr(ip_addr);
    if (addr_list.isEmpty()) {
        addr = "None";
    } else {
        addr = addr_list.first().ip().toString();
    }
    if (addr != ip_addr) {
        qCDebug(CLASS_LC) << "IP address changed to" << addr;
        ip_addr = addr;
        emit ip_addr_changed(ip_addr);
    }
}

/*****************************************************************************/
