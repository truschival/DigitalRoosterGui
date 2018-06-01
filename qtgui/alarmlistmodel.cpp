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
#include "configuration_manager.hpp"
#include "alarmlistmodel.hpp"

using namespace DigitalRooster;

/*************************************************************************************/
AlarmListModel::AlarmListModel(
    std::shared_ptr<DigitalRooster::ConfigurationManager> confman,  QObject* parent)
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

    roles[PeriodRole] = "station_name";
    roles[UriRole] = "uri";
    roles[TimeRole] = "pub_date";
    roles[EnabledRole] = "enabled";
    return roles;
}

/*************************************************************************************/

int AlarmListModel::rowCount(const QModelIndex& /*parent */) const {
    qDebug() << __FUNCTION__;
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
    case PeriodRole:
        return QVariant("periodicity");
    case UriRole:
        return QVariant("some uri");
    case TimeRole:
        return QVariant("Datestring - makes no sense");
    case EnabledRole:
        return QVariant(alarm->is_enabled());
    }

    return QVariant();
}