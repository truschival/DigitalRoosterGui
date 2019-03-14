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

#ifndef _WIFI_CONTROL_HPP_
#define _WIFI_CONTROL_HPP_

#include <QObject>
#include <QString>
#include <QStringRef>
#include <QVector>
#include <memory>


#include "wpa_ctrl/wpa_ctrl.h"


namespace DigitalRooster {
// Forward declaration
class ConfigurationManager;

/**
 * Function to register as callback
 * @param buf
 * @param len
 */
void wpa_msg_cb(char* buf, size_t len);

class WifiNetwork;
/**
 * Abstracts access to wpa_ctrl code
 * I can't believe I would use a singleton but I finally found a use case!
 * We need to register a C-Function with the wpa_ctrl.c for notification
 * Member functions are not allowed and I dont want multiple objects
 * accessing the wpa_cli at once - Singleton to the help!
 */
class WifiControl : public QObject {
    Q_OBJECT;
    friend void DigitalRooster::wpa_msg_cb(char* buf, size_t len);

public:
    /**
     * Singleton Creator method
     * @param cm configuration manager (to get WIFI interface)
     * @return
     */
    static WifiControl& get_instance(ConfigurationManager* cm = nullptr);
    /**
     * Trigger Push Button Configuration (PBC) authentication
     * with given network
     * @param network
     */
    Q_INVOKABLE void wps_pbc_auth(WifiNetwork& network);

    /**
     * Read the last scan result
     * @return list of networks found
     */
    const QVector<WifiNetwork>& get_scan_result();

    /**
     * Destructor must close wpa_cli
     */
    virtual ~WifiControl();

    /**
     * Connect to wpa control interface socket
     */
    void connect_wpa_control_socket();

public slots:
    void start_scan();

signals:
    /**
     * Scan was successful and result is available
     */
    void networks_found(const QVector<WifiNetwork>& network);
    /**
     * Scan was successful and result is available
     */
    void scan_finished();

private:
    /**
     * Construct with path to event interface for rotary encoder and
     * push button GPIO
     */
    WifiControl(QObject* parent = nullptr);

    /**
     * Holds last scan result
     */
    QVector<WifiNetwork> scan_results;
    /**
     * Handle for lower layer wpa_ctrl
     */
    struct wpa_ctrl* ctrl;

    /**
     * Path to management socket e.g. /var/lib/wpa_supplicant/wlan0
     */
    QString wpa_supplicant_sock_path;

    /**
     * Parse wpa layer notifications
     * @param buffer notification string
     * @param len size of buffer
     */
    void wpa_notification(char* buffer, size_t len);
};


/**
 * Tokenize a single line of scan results and interpret fields as WifiNetwork
 * @param line refernce to string in scan_results
 * @return WifiNetwork data object
 */
WifiNetwork line_to_network(const QStringRef& line);

/**
 * Tokenize a buffer returend by wpa_ctrl SCAN_RESULT into a vector of
 * WifiNetworks
 * @param buffer result
 * @param len buffer length
 */
QVector<WifiNetwork> parse_scanresult(const char* buffer, size_t len);


/**
 * POD class for Wifi network information
 */
class WifiNetwork {
public:
    QString name;
	QString bssid;
    int signal_strength;
    bool wps_available;
};

} // namespace DigitalRooster

#endif /* _WIFI_CONTROL_HPP_ */
