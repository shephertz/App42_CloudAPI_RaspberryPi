//
//  EventService.cpp
//  App42Cocos2dXSample
//
//  Created by Rajeev Ranjan on 02/07/15.
//
//

#include "EventService.h"
#include "App42EventResponse.h"
#include "App42Constants.h"
#include "cJSON.h"
#include <map>
#include "Signing.h"
#include "BodyBuilder.h"
#include "Connector.h"
#include "Exceptions.h"

#define RESOURCE    @"event"

#define LAST_COMMUNICATED_ON        "lastCommunicatedToApp42"
#define APP42_INSTALLED_ON          "installedApp42On"
#define APP42_EVENT_OBJECT          "app42EventObject"
#define APP42_LAST_COMMUNICATED_AT  "app42_lastCommunicatedAt"

#define APP42_INSTALL_PROPERTIES    "installProperties"
#define APP42_OLD_PROPERTIES        "oldProperties"
#define APP42_SUPER_PROPERTIES      "superProperties"
#define APP42_USER_PROPERTIES       "userProperties"
#define APP42_EVENT_NAME            "eventName"
#define APP42_ACTION                "action"
#define APP42_USER_UPDATED_ON       "app42_userUpdatedOn"
#define APP42_COUNTER               "app42_counter"
#define APP42_LOGGED_IN_USER        "app42_loggedIn_user"

using namespace App42Network;
// define the static..
EventService* EventService::_instance = NULL;

static int app42RequestCounter = 0;

EventService* EventService::Initialize(string apikey, string secretkey)
{
    if(_instance == NULL)
    {
        _instance = new EventService();
    }
    _instance->Init(apikey, secretkey);
    return _instance;
}

EventService* EventService::getInstance()
{
    return _instance;
}

void EventService::Terminate()
{
    if(_instance != NULL)
    {
        delete _instance;
        _instance = NULL;
    }
}

EventService::EventService()
{
    
}

string EventService::BuildSetPropBody(App42Object *properties)
{
    
    cJSON *bodyJSON = cJSON_CreateObject();
    cJSON *eventJSON = cJSON_CreateObject();
    cJSON *sJSON = cJSON_CreateObject();
    
    if (properties)
    {
        cJSON_AddItemReferenceToObject(sJSON, APP42_INSTALL_PROPERTIES, properties->getJson());
    }
    
    string installAt = "";
    if (installAt.length())
    {
        cJSON_AddStringToObject(sJSON, "app42_installAt", installAt.c_str());
    }
    else
    {
        cJSON_AddStringToObject(sJSON, "app42_installAt", "");
    }
    
    cJSON_AddItemReferenceToObject(eventJSON, "event", sJSON);
    cJSON_AddItemReferenceToObject(bodyJSON, "app42", eventJSON);
    
    char *cptrFormatted = cJSON_PrintUnformatted(bodyJSON);
    string bodyString = cptrFormatted;
    
    cJSON_Delete(bodyJSON);
    cJSON_Delete(eventJSON);
    cJSON_Delete(sJSON);
    printf("bodyString = %s",bodyString.c_str());
    free(cptrFormatted);
    return bodyString;
    
}


string EventService::BuildUpdatePropBody(App42Object *properties)
{
    
    cJSON *bodyJSON = cJSON_CreateObject();
    cJSON *eventJSON = cJSON_CreateObject();
    cJSON *sJSON = cJSON_CreateObject();
    
    if (properties)
    {
        cJSON_AddItemReferenceToObject(sJSON, APP42_INSTALL_PROPERTIES, properties->getJson());
    }
    
    if (properties) //Find Old prop to set
    {
        cJSON_AddItemReferenceToObject(sJSON, APP42_OLD_PROPERTIES, properties->getJson());
    }
    
    string installAt = "";
    if (installAt.length())
    {
        cJSON_AddStringToObject(sJSON, "app42_installAt", installAt.c_str());
    }
    else
    {
        cJSON_AddStringToObject(sJSON, "app42_installAt", "");
    }
    
    string userUpdatedOn = "";
    if (userUpdatedOn.length())
    {
        cJSON_AddStringToObject(sJSON, APP42_USER_UPDATED_ON, userUpdatedOn.c_str());
    }
    else
    {
        cJSON_AddStringToObject(sJSON, APP42_USER_UPDATED_ON, "");
    }
    
    string lastCommunicatedAt = "";
    if (lastCommunicatedAt.length())
    {
        cJSON_AddStringToObject(sJSON, APP42_LAST_COMMUNICATED_AT, lastCommunicatedAt.c_str());
    }
    else
    {
        cJSON_AddStringToObject(sJSON, APP42_LAST_COMMUNICATED_AT, "");
    }
    
    cJSON_AddItemReferenceToObject(eventJSON, "event", sJSON);
    cJSON_AddItemReferenceToObject(bodyJSON, "app42", eventJSON);
    
    char *cptrFormatted = cJSON_PrintUnformatted(bodyJSON);
    string bodyString = cptrFormatted;
    
    cJSON_Delete(bodyJSON);
    cJSON_Delete(eventJSON);
    cJSON_Delete(sJSON);
    printf("bodyString = %s",bodyString.c_str());
    free(cptrFormatted);
    return bodyString;
    
}


string EventService::BuildTrackEventBody(string eventName, App42Object *properties)
{
    
    cJSON *bodyJSON = cJSON_CreateObject();
    cJSON *eventJSON = cJSON_CreateObject();
    cJSON *sJSON = cJSON_CreateObject();
    
    if (eventName.length())
    {
        cJSON_AddStringToObject(sJSON, APP42_EVENT_NAME, eventName.c_str());
    }
    
    if (properties)
    {
        cJSON_AddItemReferenceToObject(sJSON, APP42_USER_PROPERTIES, properties->getJson());
    }
    
    App42Object *superProps = this->GetSuperProperties();
    if (superProps)
    {
        cJSON_AddItemReferenceToObject(sJSON, APP42_SUPER_PROPERTIES, superProps->getJson());
    }
    
    App42Object *installProps = this->GetInstallProperties();
    if (installProps)
    {
        cJSON_AddItemReferenceToObject(sJSON, APP42_INSTALL_PROPERTIES, installProps->getJson());
    }

    string lastCommunicatedAt = "";
    if (lastCommunicatedAt.length())
    {
        cJSON_AddStringToObject(sJSON, APP42_LAST_COMMUNICATED_AT, lastCommunicatedAt.c_str());
    }
    else
    {
        cJSON_AddStringToObject(sJSON, APP42_LAST_COMMUNICATED_AT, "");
    }
    
    string userUpdatedOn = "";
    if (userUpdatedOn.length())
    {
        cJSON_AddStringToObject(sJSON, APP42_USER_UPDATED_ON, userUpdatedOn.c_str());
    }
    else
    {
        cJSON_AddStringToObject(sJSON, APP42_USER_UPDATED_ON, "");
    }
    
    cJSON_AddItemReferenceToObject(eventJSON, "event", sJSON);
    cJSON_AddItemReferenceToObject(bodyJSON, "app42", eventJSON);
    
    char *cptrFormatted = cJSON_PrintUnformatted(bodyJSON);
    string bodyString = cptrFormatted;
    
    cJSON_Delete(bodyJSON);
    cJSON_Delete(eventJSON);
    cJSON_Delete(sJSON);
    printf("bodyString = %s",bodyString.c_str());
    free(cptrFormatted);
    return bodyString;
    
}

App42Object* EventService::GetSuperProperties()
{
    App42Object *app42Object = new App42Object();
    app42Object->setObject("app42_os", this->getPlatformName().c_str());
    //app42Object->setObject("app42_os_version", "");
    
    app42Object->setObject("app42_lib_version", VERSION);
    app42Object->setObject("app42_counter", app42RequestCounter);
    app42Object->setObject("app42_installAt", "");
    app42Object->setObject(APP42_LAST_COMMUNICATED_AT, "");
    app42Object->setObject("app42_app_version", "");
    return app42Object;
}

void EventService::updateApp42RequestCounter(int counter)
{
    app42RequestCounter += counter;
    if (app42RequestCounter<0) {
        app42RequestCounter = 0;
    }
}

string EventService::getPlatformName()
{
    string platform = "Raspberry Pi";
    return platform;
}

App42Object* EventService::GetInstallProperties()
{
    App42Object *app42Object = new App42Object();
    app42Object->setObject("app42_os", this->getPlatformName().c_str());
    app42Object->setObject("app42_os_version", "");
    
    
    app42Object->setObject("app42_lib_version", VERSION);
    app42Object->setObject("app42_counter", "");
    app42Object->setObject("app42_installAt", "");
    app42Object->setObject(APP42_LAST_COMMUNICATED_AT, "");
    app42Object->setObject("app42_app_version", "");
    app42Object->setObject("app42_carrier", "");
    return app42Object;
}

void EventService::TrackEvent(const char* eventName, App42Object *properties, const app42CallBack& pSelector)
{
    App42EventResponse *response = new App42EventResponse(pSelector);
    try
    {
        Util::throwExceptionIfStringNullOrBlank(eventName, "Event Name");
        Util::throwExceptionIfCallBackIsNull(pSelector, "Callback");
    }
    catch (App42Exception *e)
    {
        std::string ex = e->what();
        response->httpErrorCode = e->getHttpErrorCode();
        response->appErrorCode  = e->getAppErrorCode();
        response->errorDetails  = ex;
        response->isSuccess = false;
        if ( pSelector)
        {
            pSelector( response);
        }
        delete e;
        e = NULL;
        return;
    }
    
    map<string, string> postMap;
    populateSignParams(postMap);
    string createEventbody = BuildTrackEventBody(eventName, properties);
    postMap["body"] = createEventbody;
    
    string signature = Util::signMap(secretKey, postMap);
    
    string baseUrl = getAnalyticsUrl("event");
    baseUrl.append("?");
    
    std::vector<std::string> headers;
    map<string, string> metaHeaders;
    populateMetaHeaderParams(metaHeaders);
    Util::BuildHeaders(metaHeaders, headers);
    
    string timestamp = Util::getTimeStamp();
    Util::BuildHeaders(apiKey, timestamp, VERSION, signature, headers);
    
    Util::executePost(baseUrl, headers, createEventbody.c_str(), std::bind(&App42EventResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
}

void EventService::SetLoggedInUserProperties(App42Object *properties, const app42CallBack& pSelector)
{
    App42Response *response = new App42Response(pSelector);
    try
    {
        Util::throwExceptionIfObjectIsNull(properties, "Properties");
        Util::throwExceptionIfCallBackIsNull(pSelector, "Callback");
    }
    catch (App42Exception *e)
    {
        std::string ex = e->what();
        response->httpErrorCode = e->getHttpErrorCode();
        response->appErrorCode  = e->getAppErrorCode();
        response->errorDetails  = ex;
        response->isSuccess = false;
        if ( pSelector)
        {
            pSelector( response);
        }
        delete e;
        e = NULL;
        return;
    }
    updateApp42RequestCounter(1);
    
    map<string, string> postMap;
    populateSignParams(postMap);
    string createEventbody = BuildSetPropBody(properties);
    postMap["body"] = createEventbody;
    
    string signature = Util::signMap(secretKey, postMap);
    
    string resource = "event/";
    resource.append("userproperties");
    string baseUrl = getAnalyticsUrl(resource);
    baseUrl.append("?");
    
    std::vector<std::string> headers;
    map<string, string> metaHeaders;
    populateMetaHeaderParams(metaHeaders);
    Util::BuildHeaders(metaHeaders, headers);
    
    string timestamp = Util::getTimeStamp();
    Util::BuildHeaders(apiKey, timestamp, VERSION, signature, headers);
    
    Util::executePost(baseUrl, headers, createEventbody.c_str(), std::bind(&App42Response::onComplete, response, std::placeholders::_1, std::placeholders::_2));
}


void EventService::UpdateLoggedInUserProperties(App42Object *properties, const app42CallBack& pSelector)
{
    App42Response *response = new App42Response(pSelector);
    try
    {
        Util::throwExceptionIfObjectIsNull(properties, "Properties");
        Util::throwExceptionIfCallBackIsNull(pSelector, "Callback");
    }
    catch (App42Exception *e)
    {
        std::string ex = e->what();
        response->httpErrorCode = e->getHttpErrorCode();
        response->appErrorCode  = e->getAppErrorCode();
        response->errorDetails  = ex;
        response->isSuccess = false;
        if ( pSelector)
        {
            pSelector( response);
        }
        delete e;
        e = NULL;
        return;
    }
    updateApp42RequestCounter(1);
    
    map<string, string> postMap;
    populateSignParams(postMap);
    string createEventbody = BuildUpdatePropBody(properties);
    postMap["body"] = createEventbody;
    
    string signature = Util::signMap(secretKey, postMap);
    
    string resource = "event/";
    resource.append("updateuserproperties");
    string baseUrl = getAnalyticsUrl(resource);
    baseUrl.append("?");
    
    std::vector<std::string> headers;
    map<string, string> metaHeaders;
    populateMetaHeaderParams(metaHeaders);
    Util::BuildHeaders(metaHeaders, headers);
    
    string timestamp = Util::getTimeStamp();
    Util::BuildHeaders(apiKey, timestamp, VERSION, signature, headers);
    
    Util::executePost(baseUrl, headers, createEventbody.c_str(), std::bind(&App42Response::onComplete, response, std::placeholders::_1, std::placeholders::_2));
}


