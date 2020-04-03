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
#ifndef REST_SYSTEMAPI_HPP_
#define REST_SYSTEMAPI_HPP_

#include "ITimeoutStore.hpp"

namespace DigitalRooster {
namespace REST {

    class SystemAPI {
    public:
        SystemAPI(ITimeOutStore& tos, Pistache::Rest::Router& router);
        virtual ~SystemAPI() = default;



    private:
        ITimeOutStore& timeoutstore;
    };
} /* namespace REST */
} /* namespace DigitalRooster */

#endif /* REST_SYSTEMAPI_HPP_ */
