/*************************************************************************************
 * \filename
 * \brief
 *
 * \details
 *
 * \author Thomas Ruschival
 * \copyright 2018 Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 2 or later
 *
 * 			 SPDX-License-Identifier: GPL-2.0-or-later}
 *************************************************************************************/
#include <QByteArray>
#include <QDebug>
#include <QHash>
#include <QQmlEngine>

#include "alarm.hpp"
#include "alarmlistmodel.hpp"
#include "configuration_manager.hpp"

using namespace DigitalRooster;

/*************************************************************************************/
AlarmListModel::AlarmListModel(
    std::shared_ptr<DigitalRooster::ConfigurationManager> confman,
    QObject* parent)
    : QAbstractListModel(parent)
    , cm(confman) {
}

/*************************************************************************************/
AlarmListModel::AlarmListModel(QObject* parent)
    : QAbstractListModel(parent)
    , cm(nullptr) {
}

/*************************************************************************************/

QHash<int, QByteArray> AlarmListModel::roleNames() const {
    QHash<int, QByteArray> roles;

    roles[PeriodicityRole] = "periodicity";
    roles[PeriodStringRole] = "periodstring";
    roles[UriRole] = "uri";
    roles[TimeRole] = "triggerTime";
    roles[EnabledRole] = "alarmEnabled";
    return roles;
}

/*************************************************************************************/

int AlarmListModel::rowCount(const QModelIndex& /*parent */) const {
    if (cm->get_alarms().size() <= 0) {
        qWarning() << " alarms configured ";
    }
    return cm->get_alarms().size();
}

/*************************************************************************************/
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

/*************************************************************************************/
void AlarmListModel::set_enabled(int row, bool enabled) {
    if (row < 0 || row >= cm->get_alarms().size()) {
        qWarning() << "Invalid Selection";
        return;
	}
    cm->get_alarms().at(row)->enable(enabled);
   
	dataChanged(index(row, 0), index(row, 0),
        {TimeRole, EnabledRole, PeriodicityRole, UriRole});
}

/*************************************************************************************/

