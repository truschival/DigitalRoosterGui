/******************************************************************************
 * \filename
 * \brief	Interface QT to wpa_supplicant
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#include <QLoggingCategory>
#include <QString>
#include <exception>

#include "appconstants.hpp"
#include "configuration_manager.hpp"
#include "wifi_control.hpp"
#include "wpa_ctrl/wpa_ctrl.h"

using namespace DigitalRooster;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.WifiControl");

/****************************************************************************/

/**
 * C Callback function to receive events from wpa_ctrl.c
 * @param buf char buffer, allocated by user
 * @param len buffer length
 */
void DigitalRooster::wpa_msg_cb(char* buf, size_t len) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << buf;
    WifiControl::get_instance().wpa_notification(buf, len);
}

/****************************************************************************/
WifiControl& WifiControl::get_instance(ConfigurationManager* cm) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    static WifiControl instance;
    if (cm) {
        instance.wpa_supplicant_sock_path = cm->get_wpa_socket_name();
        instance.connect_wpa_control_socket();
    }
    return instance;
}
/****************************************************************************/
WifiControl::WifiControl(QObject* parent)
    : QObject(parent)
    , ctrl(nullptr) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}

/****************************************************************************/
void WifiControl::connect_wpa_control_socket() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << wpa_supplicant_sock_path;
    char buf[128];
    ctrl = wpa_ctrl_open(wpa_supplicant_sock_path.toStdString().c_str());
    if (!ctrl) {
        qCCritical(CLASS_LC) << " opening wpa_ctrl failed!";
    } else {
        // send ping
        size_t buf_len = sizeof(buf);
        wpa_ctrl_request(
            ctrl, "PING", sizeof("PING"), buf, &buf_len, wpa_msg_cb);
        qCDebug(CLASS_LC) << buf;
    }
}
/****************************************************************************/
WifiControl::~WifiControl() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (ctrl) {
        wpa_ctrl_close(ctrl);
    }
}
/****************************************************************************/
void WifiControl::wps_pbc_auth(WifiNetwork& network) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}

/****************************************************************************/
const QVector<WifiNetwork>& WifiControl::get_scan_result() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return scan_results;
}

/****************************************************************************/
void WifiControl::wpa_notification(char* buffer, size_t len) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    qCDebug(CLASS_LC) << buffer;
}

/****************************************************************************/
void WifiControl::start_scan() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}

/****************************************************************************/
WifiNetwork DigitalRooster::line_to_network(const QStringRef& line) {
	qCDebug(CLASS_LC) << Q_FUNC_INFO;
	QVector<QStringRef> list = line.split("\t");

    if (line.size() > 3) {
        WifiNetwork nw{
            list[4].toString(), list[0].toString(), list[2].toInt(0), false};
        return nw;
    } else {
        throw std::runtime_error("cannot parse line");
    }
}

/****************************************************************************/
QVector<WifiNetwork> DigitalRooster::parse_scanresult(
    const char* buffer, size_t len) {
    QString results(buffer);
    auto lines = results.splitRef("\n");
    QVector<WifiNetwork> cont;
    // skip first line which is header of table
    for (int i = 1; i < lines.length(); i++) {
    	try{
    		cont.push_back(line_to_network(lines[i]));
    	} catch (std::exception & e){
    		qCCritical(CLASS_LC) << e.what();
    		qCDebug(CLASS_LC) << " > " << lines[i];
    	}

    }
    return cont;
}

/****************************************************************************/
