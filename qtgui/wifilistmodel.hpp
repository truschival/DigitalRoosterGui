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

namespace DigitalRooster {
/**
 * ListModel to wifi networks in QML Gui
 */
class WifiListModel : public QAbstractListModel {
    Q_OBJECT
public:
    WifiListModel(QObject* parent = nullptr);

    enum NetworkRoles {
        BssidRole = Qt::UserRole + 1,
        NameRole,
        WpsRole,
        SignalStrengthRole,
        ConnectedRole,
    };

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    /**
     * Connect to network from list with index
     * @param index item in list
     */
    Q_INVOKABLE void wps_connect(int index);

protected:
    QHash<int, QByteArray> roleNames() const;
};
} // namespace DigitalRooster

#endif /* QTGUI_WIFILISTMODEL_HPP_ */
