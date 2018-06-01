/*************************************************************************************
 * \filename
 * \brief	Listmodel for alarms in QML Gui
 *
 * \details
 *
 * \author Thomas Ruschival
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 2 or later
 *
 * 			 SPDX-License-Identifier: GPL-2.0-or-later}
 *************************************************************************************/
#ifndef QTGUI_ALARMLISTMODEL_HPP_
#define QTGUI_ALARMLISTMODEL_HPP_

#include <QAbstractListModel>
#include <QObject>
#include <memory>

namespace DigitalRooster {
class Alarm;
class ConfigurationManager;

/**
 * ListModel to show alarms in QML Gui
 */
class AlarmListModel : public QAbstractListModel {
    Q_OBJECT
public:
    AlarmListModel(QObject* parent = nullptr);

    AlarmListModel(
        std::shared_ptr<DigitalRooster::ConfigurationManager> confman,
        QObject* parent = nullptr);

    enum AlarmRoles {
        PeriodicityRole = Qt::UserRole + 1,
        UriRole,
        TimeRole,
		EnabledRole
    };

    int rowCount(const QModelIndex& parent = QModelIndex()) const;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  
	Q_INVOKABLE void set_enabled(int row, bool enabled);
    
protected:
    QHash<int, QByteArray> roleNames() const;

private:
    std::shared_ptr<DigitalRooster::ConfigurationManager> cm;
};
} // namespace DigitalRooster

#endif /* QTGUI_ALARMLISTMODEL_HPP_ */