/*************************************************************************************
 * \filename
 * \brief	Internet Radiostation Listmodel
 *
 * \details needed to bind a list of episodes to QML
 *
 * \author Thomas Ruschival
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 *
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *************************************************************************************/
#ifndef QTGUI_IRADIOLISTMODEL_HPP_
#define QTGUI_IRADIOLISTMODEL_HPP_

#include <QAbstractListModel>
#include <QObject>
#include <QUrl>
#include <memory>

#include "IStationStore.hpp"

namespace DigitalRooster {
class PlayableItem;
class MediaPlayer;

/**
 * Simple Model for displaying Internet Radio stations in QML List
 */
class IRadioListModel : public QAbstractListModel {
    Q_OBJECT
public:
    /**
     * Constructor
     * @param store cannot be a refernce because Object must be default
     * constructable in QML/MOC Code
     * @param store configuration storage interface \ref
     * DigitalRooster::ConfigurationManager
     * @param mp Player
     * @param parent
     */
    IRadioListModel(IStationStore& store, DigitalRooster::MediaPlayer& mp,
        QObject* parent = nullptr);

    enum IRadioStationRoles { StationNameRole = Qt::UserRole + 1, UriRole };

    int rowCount(const QModelIndex& parent = QModelIndex()) const;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    Q_INVOKABLE QUrl get_station_url(int index);
    Q_INVOKABLE void send_to_player(int index);

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    IStationStore& cm;
    MediaPlayer& mpp;
};
} // namespace DigitalRooster

#endif /* QTGUI_IRADIOLISTMODEL_HPP_*/
