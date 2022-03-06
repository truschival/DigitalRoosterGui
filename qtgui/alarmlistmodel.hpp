/*************************************************************************************
 * \filename
 * \brief	Listmodel for alarms in QML Gui
 *
 * \details
 *
 * \author Thomas Ruschival
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 *
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *************************************************************************************/
#ifndef QTGUI_ALARMLISTMODEL_HPP_
#define QTGUI_ALARMLISTMODEL_HPP_

#include <QAbstractListModel>
#include <QObject>
#include <memory>

#include "IAlarmStore.hpp"

namespace DigitalRooster {
class Alarm;

/**
 * ListModel to show alarms in QML Gui
 */
class AlarmListModel : public QAbstractListModel {
    Q_OBJECT
public:
    /**
     * Constructor for AlarmListModel
     * @param store Interface to get/update/delete alarms
     * @param parent QObject hierarchy manage lifetime
     */
    explicit AlarmListModel(
        IAlarmStore& store, QObject* parent = nullptr);

    enum AlarmRoles {
        PeriodicityRole = Qt::UserRole + 1,
        PeriodStringRole,
        UriRole,
        TimeRole,
        EnabledRole
    };

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

    virtual bool removeRows(
        int row, int count, const QModelIndex& parent = QModelIndex());

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    Q_INVOKABLE void set_enabled(int row, bool enabled);

    Q_INVOKABLE DigitalRooster::Alarm* get_alarm(int);

    /**
     * Delete an alarm identified by ID from the list of alarms
     * @param id of alarm
     * @return 0 if alarm was deleted, -1 otherwise
     */
    Q_INVOKABLE int delete_alarm(qint64 id);

    /**
     * Default-create an alarm and add it to the list
     * @return pointer o newly created alarm
     */
    Q_INVOKABLE DigitalRooster::Alarm* create_alarm();

    /** Hook to request update of model */
    Q_INVOKABLE void update_row(int);

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    IAlarmStore&  config;

    /**
     * Check if valid row was selected
     * @param row current row
     * @return true if row corresponds to alarm in store
     */
    bool check_selection(int row) const;
};
} // namespace DigitalRooster

#endif /* QTGUI_ALARMLISTMODEL_HPP_ */
