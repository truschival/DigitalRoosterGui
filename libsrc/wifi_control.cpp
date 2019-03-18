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
WifiControl* WifiControl::get_instance(ConfigurationManager* cm) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    static WifiControl instance;
    // only if not initialized and we have a control_manager
    if (!instance.ctrl && cm) {
        instance.wpa_supplicant_sock_path = cm->get_wpa_socket_name();
        try {
            instance.connect_wpa_control_socket();
            instance.ctrl_notifier = std::make_unique<QSocketNotifier>(
                wpa_ctrl_get_fd(instance.ctrl), QSocketNotifier::Read);
            connect(instance.ctrl_notifier.get(), SIGNAL(activated(int)), &instance,
                SLOT(ctrl_event(int)));

        } catch (std::exception& exc) {
            qCCritical(CLASS_LC) << exc.what();
        }
    }
    return &instance;
}
/****************************************************************************/
WifiControl::WifiControl(QObject* parent)
    : QObject(parent)
    , ctrl(nullptr)
    , reply_size(sizeof(reply)) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}

/****************************************************************************/
void WifiControl::connect_wpa_control_socket() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << wpa_supplicant_sock_path;
    ctrl = wpa_ctrl_open(wpa_supplicant_sock_path.toStdString().c_str());
    if (!ctrl) {
        throw std::runtime_error("Opening wpa_ctrl failed!");
    }
    if(wpa_ctrl_attach(ctrl) < 0 ){
    	qCCritical(CLASS_LC)<< " notifier attach failed!";
    }
}
/****************************************************************************/
WifiControl::~WifiControl() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (ctrl) {
    	ctrl_notifier->setEnabled(false);
    	ctrl_notifier->disconnect();
        wpa_ctrl_detach(ctrl);
    	wpa_ctrl_close(ctrl);
    }
}
/****************************************************************************/
void WifiControl::wps_pbc_auth(const WifiNetwork& network) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    std::lock_guard<std::mutex> lock(wpa_mtx);
    QString cmd("WPS_PBC ");
    try {
        request_wrapper(cmd + network.bssid);
    } catch (std::exception& exc) {
        qCCritical(CLASS_LC) << exc.what();
    }
}

/****************************************************************************/
const QVector<WifiNetwork>& WifiControl::get_scan_result() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return scan_results;
}

/****************************************************************************/
void WifiControl::parse_event(const QString& e_string) {
	qCDebug(CLASS_LC) << Q_FUNC_INFO;
    /**
     * Did we get a scan_results event?
     */
    if (e_string.contains(WPA_EVENT_SCAN_RESULTS)) {
        qCDebug(CLASS_LC) << " scan_results available!";
        scan_finished();
        read_scan_results();
        networks_found(scan_results);
    }
    if (e_string.contains(WPA_EVENT_CONNECTED)) {
        qCDebug(CLASS_LC) << " connected!";
    }
    if (e_string.contains(WPS_EVENT_AP_AVAILABLE_PBC)) {
        qCDebug(CLASS_LC) << " WPS PBC available!";
    }
    if (e_string.contains(WPA_EVENT_DISCONNECTED)) {
        qCDebug(CLASS_LC) << " disconnected!";
    }
    if (e_string.contains(WPS_EVENT_ACTIVE)) {
        qCDebug(CLASS_LC) << " Push button Configuration active!";
    }
}

/****************************************************************************/
void WifiControl::ctrl_event(int fd){
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    char buf[128] = {};
    while (wpa_ctrl_pending(ctrl) > 0) {
        auto buf_len = sizeof(buf);
        wpa_ctrl_recv(ctrl, buf, &buf_len);
        auto e_string = QString::fromLocal8Bit(buf,buf_len);
        qCDebug(CLASS_LC) << "[monitor] CTRL:" << e_string << ":" << buf_len;
        parse_event(e_string);
    }
}

/****************************************************************************/
void WifiControl::request_wrapper(const QString& cmd) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << cmd;
    assert(ctrl);
    size_t buf_size = sizeof(reply);
    auto res = wpa_ctrl_request(
        ctrl, cmd.toStdString().c_str(), cmd.size(), reply, &buf_size, NULL);
    if(res < 0){
    	throw std::runtime_error("wpa_ctrl_request failed");
    }
}

/****************************************************************************/
void WifiControl::read_scan_results() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    assert(ctrl);
    std::lock_guard<std::mutex> lock(wpa_mtx);
    try{
    	request_wrapper("SCAN_RESULTS");
    	scan_results = parse_scanresult(reply, reply_size);
    }catch(std::exception & exc){
    	qCCritical(CLASS_LC) << exc.what();
    }
}

/****************************************************************************/
void WifiControl::start_scan() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    assert(ctrl);
    std::lock_guard<std::mutex> lock(wpa_mtx);
    try{
    	request_wrapper("SCAN");
    }catch(std::exception & exc){
    	qCCritical(CLASS_LC) << " SCAN:" << exc.what();
    }
}

/****************************************************************************/
WifiNetwork DigitalRooster::line_to_network(const QStringRef& line) {
	qCDebug(CLASS_LC) << Q_FUNC_INFO;
	QVector<QStringRef> list = line.split("\t");

    if (list.size() > 3) {
        WifiNetwork nw{
            list[4].toString(), list[0].toString(), list[2].toInt(0), false};
        return nw;
    } else {
        throw std::runtime_error("parse error");
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
