/******************************************************************************
 * \filename
 * \brief     API for managing Internet Radio Stations
 *
 * \details
 *
 * \copyright (c) 2020  Thomas Ruschival <thomas@ruschival.de>
 * \license {This file is licensed under GNU PUBLIC LICENSE Version 3 or later
 * 			 SPDX-License-Identifier: GPL-3.0-or-later}
 *
 *****************************************************************************/
#include <QLoggingCategory>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include <pistache/endpoint.h>
#include <pistache/http.h>

#include "AlarmApi.hpp"
#include "alarm.hpp"
#include "common.hpp"

using namespace Pistache;
using namespace Pistache::Rest;

using namespace DigitalRooster;
using namespace DigitalRooster::REST;

static Q_LOGGING_CATEGORY(CLASS_LC, "AlarmApi");

/*****************************************************************************/
AlarmApi::AlarmApi(IAlarmStore& as, Pistache::Rest::Router& router)
    : alarmstore(as) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    // Access list or create new station
    Routes::Get(router, API_URL_BASE + api_ressource,
        Routes::bind(&AlarmApi::read_alarm_list, this));
    Routes::Post(router, API_URL_BASE + api_ressource,
        Routes::bind(&AlarmApi::add_alarm, this));

    // Manage individual station identified by UUID
    Routes::Get(router, API_URL_BASE + api_ressource + "/:uid",
        Routes::bind(&AlarmApi::get_alarm, this));
    Routes::Delete(router, API_URL_BASE + api_ressource + "/:uid",
        Routes::bind(&AlarmApi::delete_alarm, this));
}

/*****************************************************************************/
// coverity[PASS_BY_VALUE]
void AlarmApi::read_alarm_list(const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    respond_json_array(alarmstore.get_alarms(), request, response);
}

/*****************************************************************************/
void AlarmApi::get_alarm(const Pistache::Rest::Request& request,
    // coverity[PASS_BY_VALUE]
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    try {
        // Massively ugly casting to get something convertible to QUuid...
        auto uid = QUuid::fromString(
            QLatin1String(request.param(":uid").as<std::string>().c_str()));
        QJsonDocument jd;
        auto result = alarmstore.get_alarm(uid);
        jd.setObject(result->to_json_object());
        response.send(Pistache::Http::Code::Ok, jd.toJson().toStdString());
    } catch (std::out_of_range& oor) {
        // wrong UUID provided
        response.send(
            Pistache::Http::Code::Bad_Request, BAD_REQUEST_NO_ITEM_WITH_UUID);
    } catch (std::exception& exc) {
        // some other error occurred -> 500
        InternalErrorJson je(exc, 500);
        response.send(Pistache::Http::Code::Internal_Server_Error, je);
    }
}

/*****************************************************************************/
void AlarmApi::add_alarm(const Pistache::Rest::Request& request,
    /* coverity[PASS_BY_VALUE] */
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    try {
        auto alm =
            Alarm::from_json_object(qjson_form_std_string(request.body()));
        alarmstore.add_alarm(alm);
        respond_SuccessCreated(alm, response);
    } catch (std::invalid_argument& ia) {
        InternalErrorJson je(ia, 400);
        response.send(Pistache::Http::Code::Bad_Request, je);
    } catch (std::exception& exc) {
        // some other error occurred -> 500
        InternalErrorJson je(exc, 500);
        response.send(Pistache::Http::Code::Internal_Server_Error, je);
    }
}

/*****************************************************************************/
/* coverity[PASS_BY_VALUE] - Pistache API does a std::move down the line */
void AlarmApi::delete_alarm(const Pistache::Rest::Request& request,
    Pistache::Http::ResponseWriter response) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    try {
        // Massively ugly casting to get something convertible to QUuid...
        auto uid = QUuid::fromString(
            QLatin1String(request.param(":uid").as<std::string>().c_str()));
        alarmstore.delete_alarm(uid);
        response.send(Pistache::Http::Code::Ok);
    } catch (std::out_of_range& oor) {
        // wrong UUID provided
        response.send(
            Pistache::Http::Code::Bad_Request, BAD_REQUEST_NO_ITEM_WITH_UUID);
    } catch (std::exception& exc) {
        // some other error occurred -> 500
        InternalErrorJson je(exc, 500);
        response.send(Pistache::Http::Code::Internal_Server_Error, je);
    }
}
