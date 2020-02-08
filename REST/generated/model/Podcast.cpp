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


#include "Podcast.h"

namespace org {
namespace openapitools {
namespace server {
namespace model {

Podcast::Podcast()
{
    m_Uid = "";
    m_UidIsSet = false;
    m_Name = "";
    m_Url = "";
    m_UpdateInterval = 0;
    m_UpdateIntervalIsSet = false;
    
}

Podcast::~Podcast()
{
}

void Podcast::validate()
{
    // TODO: implement validation
}

void to_json(nlohmann::json& j, const Podcast& o)
{
    j = nlohmann::json();
    if(o.uidIsSet())
        j["uid"] = o.m_Uid;
    j["name"] = o.m_Name;
    j["url"] = o.m_Url;
    if(o.updateIntervalIsSet())
        j["updateInterval"] = o.m_UpdateInterval;
}

void from_json(const nlohmann::json& j, Podcast& o)
{
    if(j.find("uid") != j.end())
    {
        j.at("uid").get_to(o.m_Uid);
        o.m_UidIsSet = true;
    } 
    j.at("name").get_to(o.m_Name);
    j.at("url").get_to(o.m_Url);
    if(j.find("updateInterval") != j.end())
    {
        j.at("updateInterval").get_to(o.m_UpdateInterval);
        o.m_UpdateIntervalIsSet = true;
    } 
}

std::string Podcast::getUid() const
{
    return m_Uid;
}
void Podcast::setUid(std::string const& value)
{
    m_Uid = value;
    m_UidIsSet = true;
}
bool Podcast::uidIsSet() const
{
    return m_UidIsSet;
}
void Podcast::unsetUid()
{
    m_UidIsSet = false;
}
std::string Podcast::getName() const
{
    return m_Name;
}
void Podcast::setName(std::string const& value)
{
    m_Name = value;
}
std::string Podcast::getUrl() const
{
    return m_Url;
}
void Podcast::setUrl(std::string const& value)
{
    m_Url = value;
}
int32_t Podcast::getUpdateInterval() const
{
    return m_UpdateInterval;
}
void Podcast::setUpdateInterval(int32_t const value)
{
    m_UpdateInterval = value;
    m_UpdateIntervalIsSet = true;
}
bool Podcast::updateIntervalIsSet() const
{
    return m_UpdateIntervalIsSet;
}
void Podcast::unsetUpdateInterval()
{
    m_UpdateIntervalIsSet = false;
}

}
}
}
}

