/*******************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \author Thomas Ruschival
 * \copyright 2018 Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 *
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 ******************************************************************************/
#include <QByteArray>
#include <QDebug>
#include <QHash>
#include <QQmlEngine>

#include "alarm.hpp"
#include "alarmlistmodel.hpp"
#include "configuration_manager.hpp"

using namespace DigitalRooster;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.AlarmListModel");

/******************************************************************************/

AlarmListModel::AlarmListModel(
    std::shared_ptr<DigitalRooster::ConfigurationManager> confman,
    QObject* parent)
    : QAbstractListModel(parent)
    , cm(confman) {
}

/******************************************************************************/

AlarmListModel::AlarmListModel(QObject* parent)
    : QAbstractListModel(parent)
    , cm(nullptr) {
}

/******************************************************************************/

QHash<int, QByteArray> AlarmListModel::roleNames() const {
    QHash<int, QByteArray> roles;

    roles[PeriodicityRole] = "periodicity";
    roles[PeriodStringRole] = "periodstring";
    roles[UriRole] = "uri";
    roles[TimeRole] = "triggerTime";
    roles[EnabledRole] = "alarmEnabled";
    return roles;
}
/******************************************************************************/

int AlarmListModel::rowCount(const QModelIndex& /*parent */) const {
    if (cm->get_alarms().size() <= 0) {
        qWarning() << " alarms configured ";
    }
    return cm->get_alarms().size();
}

/******************************************************************************/
QVariant AlarmListModel::data(const QModelIndex& index, int role) const {
    if (cm->get_alarms().size() <= 0)
        return QVariant();

    if (index.row() < 0 || index.row() >= cm->get_alarms().size())
        return QVariant();

    auto alarm = cm->get_alarms().at(index.row());

    switch (role) {
    case PeriodicityRole:
        return QVariant(alarm->get_period());
    case PeriodStringRole:
        return QVariant(alarm->get_period_string());
    case UriRole:
        return QVariant("some uri");
    case TimeRole:
        return QVariant(alarm->get_time());
    case EnabledRole:
        return QVariant(alarm->is_enabled());
    }

    return QVariant();
}
/******************************************************************************/
void AlarmListModel::set_enabled(int row, bool enabled) {
    if (row < 0 || row >= cm->get_alarms().size()) {
        qWarning() << "Invalid Selection";
        return;
    }
    cm->get_alarms().at(row)->enable(enabled);

    emit dataChanged(index(row, 0), index(row, 0), {EnabledRole});
}

/******************************************************************************/
DigitalRooster::Alarm* AlarmListModel::get_alarm(int row) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (row < 0 || row >= cm->get_alarms().size()) {
        qWarning() << "Invalid Selection";
    }
    auto ret = cm->get_alarms().at(row).get();
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);

    return ret;
}

/******************************************************************************/
void AlarmListModel::update_row(int row) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << row;
    if (row < 0 || row >= cm->get_alarms().size()) {
        qWarning() << "Invalid Selection";
    }

    emit dataChanged(index(row, 0), index(row, 0),
        {TimeRole, EnabledRole, PeriodicityRole, PeriodStringRole, UriRole});
}

/*****************************************************************************/

bool AlarmListModel::removeRows(
        int row, int count, const QModelIndex& /*parent */){
	//cm->delete_alarm(id);
	return true;
}

/*****************************************************************************/
int AlarmListModel::delete_alarm(qint64 row) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << row;
    beginRemoveRows(QModelIndex(), row, row);
    auto alarm = cm->get_alarms().at(row);
    if(alarm){
    	cm->delete_alarm(alarm->get_id());
    }
    endRemoveRows();
    return 0;
};

/*****************************************************************************/
std::shared_ptr<DigitalRooster::Alarm> AlarmListModel::create_alarm() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto new_alarm = std::make_shared<Alarm>();
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    QQmlEngine::setObjectOwnership(new_alarm.get(), QQmlEngine::CppOwnership);
    cm->add_alarm(new_alarm);
    endInsertRows();
    return new_alarm;
}
/******************************************************************************/
