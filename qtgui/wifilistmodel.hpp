/*************************************************************************************
 * \filename
 * \brief	Listmodel for Wifi Networks
 *
 * \details
 *
 * \author Thomas Ruschival
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 *
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *************************************************************************************/
#ifndef QTGUI_WIFILISTMODEL_HPP_
#define QTGUI_WIFILISTMODEL_HPP_

#include <QAbstractListModel>
#include <QObject>

#include <vector>

#include "wifi_control.hpp"

namespace DigitalRooster {
/**
 * ListModel to wifi networks in QML Gui
 */
class WifiListModel : public QAbstractListModel {
    Q_OBJECT
public:
    explicit WifiListModel(QObject* parent = nullptr);

    enum NetworkRoles {
        BssidRole = Qt::UserRole + 1,
        NameRole,
        WpsRole,
        SignalStrengthRole,
        ConnectedRole,
    };

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

    /**
     * Returns data for role and index
     * @param index current index
     * @param role
     * @return Info as QVariant
     */
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    /**
     * Connect to network from list with index
     * @param index item in list
     */
    Q_INVOKABLE void wps_connect(int index);

public slots:
    /**
     * Clears current scan_results and replaces the list with
     * @param results
     */
    void update_scan_results(const std::vector<WifiNetwork>& results);

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    /**
     * The listmodel holds a copy of last scan results
     */
    std::vector<WifiNetwork> scan_results;
};
} // namespace DigitalRooster

#endif /* QTGUI_WIFILISTMODEL_HPP_ */
