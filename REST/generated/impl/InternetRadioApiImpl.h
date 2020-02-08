/**
* DigitalRooster
* Open API for Digital Rooster
*
* The version of the OpenAPI document: 0.8.0
* 
*
* NOTE: This class is auto generated by OpenAPI Generator (https://openapi-generator.tech).
* https://openapi-generator.tech
* Do not edit the class manually.
*/

/*
* InternetRadioApiImpl.h
*
* 
*/

#ifndef INTERNET_RADIO_API_IMPL_H_
#define INTERNET_RADIO_API_IMPL_H_


#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>
#include <memory>

#include <InternetRadioApi.h>

#include <pistache/optional.h>

#include "Error.h"
#include "Station.h"
#include <string>

namespace org {
namespace openapitools {
namespace server {
namespace api {

using namespace org::openapitools::server::model;

class InternetRadioApiImpl : public org::openapitools::server::api::InternetRadioApi {
public:
    InternetRadioApiImpl(std::shared_ptr<Pistache::Rest::Router>);
    ~InternetRadioApiImpl() {}

    void streams_create(const Station &station, Pistache::Http::ResponseWriter &response);
    void streams_delete(const std::string &uid, Pistache::Http::ResponseWriter &response);
    void streams_read_all(const Pistache::Optional<int32_t> &length, const Pistache::Optional<int32_t> &offset, Pistache::Http::ResponseWriter &response);
    void streams_read_one(const std::string &uid, Pistache::Http::ResponseWriter &response);
    void streams_update_one(const std::string &uid, const Station &station, Pistache::Http::ResponseWriter &response);

};

}
}
}
}



#endif