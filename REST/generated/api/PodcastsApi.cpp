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

#include "PodcastsApi.h"
#include "Helpers.h"

namespace org {
namespace openapitools {
namespace server {
namespace api {

using namespace org::openapitools::server::helpers;
using namespace org::openapitools::server::model;

PodcastsApi::PodcastsApi(std::shared_ptr<Pistache::Rest::Router> rtr) { 
    router = rtr;
}

void PodcastsApi::init() {
    setupRoutes();
}

void PodcastsApi::setupRoutes() {
    using namespace Pistache::Rest;

    Routes::Post(*router, base + "/podcasts", Routes::bind(&PodcastsApi::podcasts_create_handler, this));
    Routes::Delete(*router, base + "/podcasts/:uid", Routes::bind(&PodcastsApi::podcasts_delete_handler, this));
    Routes::Get(*router, base + "/podcasts", Routes::bind(&PodcastsApi::podcasts_read_all_handler, this));
    Routes::Get(*router, base + "/podcasts/:uid", Routes::bind(&PodcastsApi::podcasts_read_one_handler, this));
    Routes::Put(*router, base + "/podcasts/:uid", Routes::bind(&PodcastsApi::podcasts_update_one_handler, this));

    // Default handler, called when a route is not found
    router->addCustomHandler(Routes::bind(&PodcastsApi::podcasts_api_default_handler, this));
}

void PodcastsApi::podcasts_create_handler(const Pistache::Rest::Request &request, Pistache::Http::ResponseWriter response) {

    // Getting the body param
    
    Podcast podcast;
    
    try {
      nlohmann::json::parse(request.body()).get_to(podcast);
      this->podcasts_create(podcast, response);
    } catch (nlohmann::detail::exception &e) {
        //send a 400 error
        response.send(Pistache::Http::Code::Bad_Request, e.what());
        return;
    } catch (std::exception &e) {
        //send a 500 error
        response.send(Pistache::Http::Code::Internal_Server_Error, e.what());
        return;
    }

}
void PodcastsApi::podcasts_delete_handler(const Pistache::Rest::Request &request, Pistache::Http::ResponseWriter response) {
    // Getting the path params
    auto uid = request.param(":uid").as<std::string>();
    
    try {
      this->podcasts_delete(uid, response);
    } catch (nlohmann::detail::exception &e) {
        //send a 400 error
        response.send(Pistache::Http::Code::Bad_Request, e.what());
        return;
    } catch (std::exception &e) {
        //send a 500 error
        response.send(Pistache::Http::Code::Internal_Server_Error, e.what());
        return;
    }

}
void PodcastsApi::podcasts_read_all_handler(const Pistache::Rest::Request &request, Pistache::Http::ResponseWriter response) {

    // Getting the query params
    auto lengthQuery = request.query().get("length");
    Pistache::Optional<int32_t> length;
    if(!lengthQuery.isEmpty()){
        int32_t value;
        if(fromStringValue(lengthQuery.get(), value)){
            length = Pistache::Some(value);
        }
    }
    auto offsetQuery = request.query().get("offset");
    Pistache::Optional<int32_t> offset;
    if(!offsetQuery.isEmpty()){
        int32_t value;
        if(fromStringValue(offsetQuery.get(), value)){
            offset = Pistache::Some(value);
        }
    }
    
    try {
      this->podcasts_read_all(length, offset, response);
    } catch (nlohmann::detail::exception &e) {
        //send a 400 error
        response.send(Pistache::Http::Code::Bad_Request, e.what());
        return;
    } catch (std::exception &e) {
        //send a 500 error
        response.send(Pistache::Http::Code::Internal_Server_Error, e.what());
        return;
    }

}
void PodcastsApi::podcasts_read_one_handler(const Pistache::Rest::Request &request, Pistache::Http::ResponseWriter response) {
    // Getting the path params
    auto uid = request.param(":uid").as<std::string>();
    
    try {
      this->podcasts_read_one(uid, response);
    } catch (nlohmann::detail::exception &e) {
        //send a 400 error
        response.send(Pistache::Http::Code::Bad_Request, e.what());
        return;
    } catch (std::exception &e) {
        //send a 500 error
        response.send(Pistache::Http::Code::Internal_Server_Error, e.what());
        return;
    }

}
void PodcastsApi::podcasts_update_one_handler(const Pistache::Rest::Request &request, Pistache::Http::ResponseWriter response) {
    // Getting the path params
    auto uid = request.param(":uid").as<std::string>();
    
    // Getting the body param
    
    Podcast podcast;
    
    try {
      nlohmann::json::parse(request.body()).get_to(podcast);
      this->podcasts_update_one(uid, podcast, response);
    } catch (nlohmann::detail::exception &e) {
        //send a 400 error
        response.send(Pistache::Http::Code::Bad_Request, e.what());
        return;
    } catch (std::exception &e) {
        //send a 500 error
        response.send(Pistache::Http::Code::Internal_Server_Error, e.what());
        return;
    }

}

void PodcastsApi::podcasts_api_default_handler(const Pistache::Rest::Request &, Pistache::Http::ResponseWriter response) {
    response.send(Pistache::Http::Code::Not_Found, "The requested method does not exist");
}

}
}
}
}
