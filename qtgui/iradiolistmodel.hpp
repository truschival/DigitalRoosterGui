/*************************************************************************************
 * \filename
 * \brief	Internet Radiostation Listmodel
 *
 * \details needed to bind a list of episodes to QML
 *
 * \author Thomas Ruschival
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 2 or later
 *
 * 			 SPDX-License-Identifier: GPL-2.0-or-later}
 *************************************************************************************/
#ifndef QTGUI_IRADIOLISTMODEL_HPP_
#define QTGUI_IRADIOLISTMODEL_HPP_

#include <QAbstractListModel>
#include <QObject>
#include <QVector>
#include <memory>


namespace DigitalRooster {
	class PlayableItem;
	class MediaPlayerProxy;
	class ConfigurationManager;

	/**
	 * Simple Model for displaying Internet Radio stations in QML List
	*/
	class IRadioListModel : public QAbstractListModel {
		Q_OBJECT
	public:
		IRadioListModel(QObject* parent = nullptr);

        IRadioListModel(ConfigurationManager* confman, MediaPlayerProxy* pp,
            QObject* parent = nullptr);

        enum IRadioStationRoles {
			StationNameRole = Qt::UserRole + 1,
			UriRole,
			DateRole
		};

		int rowCount(const QModelIndex& parent = QModelIndex()) const;

		QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

		Q_INVOKABLE DigitalRooster::PlayableItem* get_station(int index);
		Q_INVOKABLE void send_to_player(int index);

	protected:
		QHash<int, QByteArray> roleNames() const;

	private:
		DigitalRooster::ConfigurationManager* cm;
		DigitalRooster::MediaPlayerProxy* mpp;
	};
}//namespace

#endif /* QTGUI_IRADIOLISTMODEL_HPP_*/
