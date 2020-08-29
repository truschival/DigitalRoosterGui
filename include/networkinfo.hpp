/******************************************************************************
 * \filename
 * \brief	 Get basic network information
 *
 * \details
 *
 * \copyright (c) 2018  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/

#ifndef _NETWORKINFO_HPP_
#define _NETWORKINFO_HPP_

#include <QObject>

namespace DigitalRooster {

/**
 * Small wrapper class around QNetworkInterface et. al
 * to conveniently access current IP address and interface status
 */
class NetworkInfo : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString ipaddr READ get_ip_addr NOTIFY ip_addr_changed)
    Q_PROPERTY(bool linkstatus READ get_link_status NOTIFY link_status_changed)
public:
    /**
     * Default Constructor
     */
    explicit NetworkInfo(QObject* parent = nullptr);

    /**
     * Constructor setting interface name
     * @param name os-name of interface (wlan0, wlp2s0)
     */
    explicit NetworkInfo(QString name, QObject* parent = nullptr);
    ~NetworkInfo() = default;

    /**
     * access WIFI interface IP address
     * @return Ip address formatted as QString
     */
    QString get_ip_addr() const;

    /**
     * access WIFI interface link status (up/down)
     * @return link status
     */
    bool get_link_status() const;

signals:
    /**
     * IP address has changed (currently not emitted)
     */
    void ip_addr_changed(QString addr);

    /**
     * Link status has changed (currently not emitted)
     */
    void link_status_changed(bool up);

private:
    /**
     * Interface name to watch
     */
    QString ifname = {"wlan0"};
};

} // namespace DigitalRooster
#endif /* _NETWORKINFO_HPP_ */
