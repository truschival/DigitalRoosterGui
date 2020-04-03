/******************************************************************************
 * \filename
 * \brief     All system global settings
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

#include <pistache/router.h>


#include "SystemAPI.hpp"
#include "common.hpp"

using namespace Pistache;
using namespace Pistache::Rest;

using namespace DigitalRooster;
using namespace DigitalRooster::REST;

static Q_LOGGING_CATEGORY(CLASS_LC, "DigitalRooster.SystemAPI");

/*****************************************************************************/
SystemAPI::SystemAPI(ITimeOutStore& tos, Pistache::Rest::Router& router)
    : timeoutstore(tos) {
	 qCDebug(CLASS_LC) << Q_FUNC_INFO;
}

/*****************************************************************************/
