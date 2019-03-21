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
#include <QSocketNotifier>
#include <QString>
#include <QStringRef>
#include <QVector>

#include <memory>
#include <mutex>
#include <thread>

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
    Q_ENUMS(ScanStatus)
    Q_PROPERTY(
        ScanStatus scan_status READ get_scan_status NOTIFY scan_status_changed)

    friend void DigitalRooster::wpa_msg_cb(char* buf, size_t len);

public:
    /**
     * Component state during scanning
     */
    enum ScanStatus {
        Idle,       //!< Never started a scan
        Scanning,   //!< currently scanning
        ScanFailed, //!< scan failed
        ScanOk      //!< Scan o.k. probably with results
    };
    Q_ENUM(ScanStatus);

    /**
     * Singleton Creator method
     * @param cm configuration manager (to get WIFI interface)
     * @return
     */
    static WifiControl* get_instance(ConfigurationManager* cm = nullptr);

    /**
     * Trigger Push Button Configuration (PBC) authentication
     * with given network
     * @param network
     */
    void wps_pbc_auth(const WifiNetwork& network);

    /**
     * Get current scan status
     * @return
     */
    ScanStatus get_scan_status() const;

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

    /**
     * Trigger a scan
     */
    Q_INVOKABLE void start_scan();

public slots:
    /**
     * data on control channel available
     */
    void ctrl_event(int fd);

signals:
    /**
     * Scan was successful and result is available
     */
    void networks_found(const QVector<WifiNetwork>& network);

    /**
     * Status changed
     */
    void scan_status_changed(ScanStatus status);

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
     * Current scan status
     */
    ScanStatus scan_stat;

    /**
     * Buffer to hold reply of command
     */
    char reply[2048];
    size_t reply_size;

    /**
     * Mutex to protect concurrent access to wpa_ctrl and buffers
     * reply or reply_size
     */
    mutable std::mutex wpa_mtx;

    /**
     * Path to management socket e.g. /var/lib/wpa_supplicant/wlan0
     */
    QString wpa_supplicant_sock_path;

    /**
     * Notifier for watching asynchornous events from wpa_ctrl socket
     */
    std::unique_ptr<QSocketNotifier> ctrl_notifier;

    /**
     * Send read scan_results from wpa_ctrl
     */
    void read_scan_results();

    /**
     * Issue a command, modifies reply and reply_size
     * @param cmd wpa_supplicant command
     */
    void request_wrapper(const QString& cmd);

    /**
     * Interpret event string from wpa_socket monitor
     * @param e_string event string
     */
    void parse_event(const QString& e_string);

    /**
     * Set scan_stat and emit signal
     * @param stat new status
     */
    void set_scan_status(ScanStatus stat);
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
    bool connected;
};

} // namespace DigitalRooster

#endif /* _WIFI_CONTROL_HPP_ */
