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
#include <QHash>
#include <QLoggingCategory>
#include <QQmlEngine>

#include "alarm.hpp"
#include "alarmlistmodel.hpp"

using namespace DigitalRooster;

static Q_LOGGING_CATEGORY(CLASS_LC, "AlarmListModel");

/******************************************************************************/
AlarmListModel::AlarmListModel(IAlarmStore& store, QObject* parent)
    : QAbstractListModel(parent)
    , cm(store) {
}

/******************************************************************************/
bool AlarmListModel::check_selection(int row) const {
    auto sz = cm.get_alarms().size();
    /* static cast only if index >= 0 and thus can be converted */
    if (row < 0 || static_cast<size_t>(row) >= sz) {
        qCCritical(CLASS_LC) << Q_FUNC_INFO << "invalid row " << row;
        return false;
    }
    return true;
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
    auto sz = cm.get_alarms().size();
    if (sz <= 0) {
        qCInfo(CLASS_LC) << "no alarms configured ";
    }
    return sz;
}

/******************************************************************************/
QVariant AlarmListModel::data(const QModelIndex& index, int role) const {
    if (!check_selection(index.row())) {
        return QVariant();
    }

    auto alarm = cm.get_alarms().at(index.row());

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
    if (!check_selection(row)) {
        return;
    }
    cm.get_alarms().at(row)->enable(enabled);

    emit dataChanged(index(row, 0), index(row, 0), {EnabledRole});
}

/******************************************************************************/
DigitalRooster::Alarm* AlarmListModel::get_alarm(int row) {
    if (!check_selection(row)) {
        qCCritical(CLASS_LC) << Q_FUNC_INFO << "invalid row!";
        return nullptr;
    }
    auto ret = cm.get_alarms().at(row).get();
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);

    return ret;
}

/******************************************************************************/
void AlarmListModel::update_row(int row) {
    if (!check_selection(row)) {
        return;
    }
    emit dataChanged(index(row, 0), index(row, 0),
        {TimeRole, EnabledRole, PeriodicityRole, PeriodStringRole, UriRole});
}

/*****************************************************************************/
bool AlarmListModel::removeRows(
    int row, int count, const QModelIndex& /*parent */) {
    qCWarning(CLASS_LC) << Q_FUNC_INFO << "is not implemented!";
    // TODO:
    // cm->delete_alarm(id);
    return true;
}

/*****************************************************************************/
int AlarmListModel::delete_alarm(qint64 row) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << row;
    beginRemoveRows(QModelIndex(), row, row);
    try {
        auto alarm = cm.get_alarms().at(row);
        if (alarm) {
            cm.delete_alarm(alarm->get_id());
        }
    } catch (std::out_of_range& exc) {
        qCWarning(CLASS_LC) << Q_FUNC_INFO << " Alarm not found! ";
    }
    endRemoveRows();
    return 0;
};

/*****************************************************************************/
DigitalRooster::Alarm* AlarmListModel::create_alarm() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    auto new_alarm = std::make_shared<Alarm>();
    /**
     * FIXME: this is bad. we create a new alarm and add it to the list
     * it instantly appears and the dialog is opened. The alarm exists even if
     * the user pressed cancel
     */
    new_alarm->set_time(QTime::fromString("06:30", "hh:mm"));
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    QQmlEngine::setObjectOwnership(new_alarm.get(), QQmlEngine::CppOwnership);
    cm.add_alarm(new_alarm);
    endInsertRows();
    return new_alarm.get();
}
/******************************************************************************/
