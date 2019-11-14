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

namespace DigitalRooster {
class Alarm;
class ConfigurationManager;

/**
 * ListModel to show alarms in QML Gui
 */
class AlarmListModel : public QAbstractListModel {
    Q_OBJECT
public:
    explicit AlarmListModel(QObject* parent = nullptr);

    explicit AlarmListModel(
        std::shared_ptr<DigitalRooster::ConfigurationManager> confman,
        QObject* parent = nullptr);

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
    std::shared_ptr<DigitalRooster::ConfigurationManager> cm;
};
} // namespace DigitalRooster

#endif /* QTGUI_ALARMLISTMODEL_HPP_ */
