//
//  PushNotificationService.cpp
//  App42CPPSDK
//
//  Created by Rajeev Ranjan on 21/10/13.
//
//

#include "PushNotificationService.h"
#include "App42Constants.h"
#include "cJSON.h"
#include <map>
#include "Signing.h"
#include "BodyBuilder.h"
#include "App42PushNotificationResponse.h"
#include "Connector.h"

using namespace App42Network;

// define the static..
PushNotificationService* PushNotificationService::_instance = NULL;

PushNotificationService* PushNotificationService::Initialize(string apikey, string secretkey)
{
	if(_instance == NULL)
    {
		_instance = new PushNotificationService();
	}
    _instance->Init(apikey, secretkey);
    return _instance;
}

PushNotificationService* PushNotificationService::getInstance()
{
	return _instance;
}

void PushNotificationService::Terminate()
{
	if(_instance != NULL)
    {
		delete _instance;
		_instance = NULL;
	}
}

PushNotificationService::PushNotificationService()
{
    
}

const char* getJsonStringFromUserList(vector<string>userList)
{
    cJSON *bodyJSON = cJSON_CreateObject();
    cJSON *userJSON = cJSON_CreateArray();
    
    std::vector<string>::iterator it;
    
    for(it=userList.begin(); it!=userList.end(); ++it)
    {
        cJSON_AddItemToArray(userJSON, cJSON_CreateString(it->c_str()));
    }
    cJSON_AddItemReferenceToObject(bodyJSON, "user", userJSON);
    
    char *cptrFormatted = cJSON_PrintUnformatted(bodyJSON);
    string bodyString = cptrFormatted;
    
    cJSON_Delete(userJSON);
    cJSON_Delete(bodyJSON);
    
    free(cptrFormatted);
    
    return bodyString.c_str();
    
}


string BuildUnsubcribeDeviceBody(string deviceToken, string userName)
{
    
    cJSON *bodyJSON = cJSON_CreateObject();
    cJSON *sJSON = cJSON_CreateObject();
    
    cJSON_AddStringToObject(sJSON, "deviceToken", deviceToken.c_str());
    cJSON_AddStringToObject(sJSON, "userName", userName.c_str());
    cJSON_AddItemReferenceToObject(bodyJSON, "push", sJSON);
    
    char *cptrFormatted = cJSON_PrintUnformatted(bodyJSON);
    string bodyString = cptrFormatted;
    
    cJSON_Delete(bodyJSON);
    cJSON_Delete(sJSON);
    
    free(cptrFormatted);
    return bodyString;
    
}


string BuildRegisterDeviceTokenBody(string deviceToken, string userName, string deviceType)
{
    
    cJSON *bodyJSON = cJSON_CreateObject();
    cJSON *pushJSON = cJSON_CreateObject();
    cJSON *sJSON = cJSON_CreateObject();
    
    cJSON_AddStringToObject(sJSON, "deviceToken", deviceToken.c_str());
    cJSON_AddStringToObject(sJSON, "type", deviceType.c_str());
    cJSON_AddStringToObject(sJSON, "userName", userName.c_str());
    cJSON_AddItemReferenceToObject(pushJSON, "push", sJSON);
    cJSON_AddItemReferenceToObject(bodyJSON, "app42", pushJSON);
    
    char *cptrFormatted = cJSON_PrintUnformatted(bodyJSON);
    string bodyString = cptrFormatted;
    
    cJSON_Delete(bodyJSON);
    cJSON_Delete(pushJSON);
    cJSON_Delete(sJSON);
    
    free(cptrFormatted);
    return bodyString;
    
}

string BuildPushBody(string userName, string message,string expiry,string deviceType, string deviceToken="")
{
    
    cJSON *bodyJSON = cJSON_CreateObject();
    cJSON *pushJSON = cJSON_CreateObject();
    cJSON *messageJson = cJSON_CreateObject();
    cJSON *sJSON = cJSON_CreateObject();
    
    if (userName.length())
    {
        cJSON_AddStringToObject(sJSON, "username", userName.c_str());
    }
    
    if (message.length())
    {
        cJSON_AddStringToObject(sJSON, "payload", message.c_str());
    }
    
    if (expiry.length())
    {
        cJSON_AddStringToObject(sJSON, "expiry", expiry.c_str());
    }
    
    if (deviceType.length())
    {
        cJSON_AddStringToObject(sJSON, "type", deviceType.c_str());
    }
    
    if (deviceToken.length())
    {
        cJSON_AddStringToObject(sJSON, "deviceId", deviceToken.c_str());
    }
    
    cJSON_AddItemReferenceToObject(messageJson, "message", sJSON);
    cJSON_AddItemReferenceToObject(pushJSON, "push", messageJson);
    cJSON_AddItemReferenceToObject(bodyJSON, "app42", pushJSON);
    
    char *cptrFormatted = cJSON_PrintUnformatted(bodyJSON);
    string bodyString = cptrFormatted;
    
    cJSON_Delete(bodyJSON);
    cJSON_Delete(pushJSON);
    cJSON_Delete(messageJson);
    cJSON_Delete(sJSON);
    
    free(cptrFormatted);
    return bodyString;
    
}

string BuildInActiveUsersPushBody(string startDate,string endDate, string message,string expiry)
{
    
    cJSON *bodyJSON = cJSON_CreateObject();
    cJSON *pushJSON = cJSON_CreateObject();
    cJSON *messageJson = cJSON_CreateObject();
    cJSON *sJSON = cJSON_CreateObject();
    
    if (startDate.length())
    {
        cJSON_AddStringToObject(sJSON, "startDate", startDate.c_str());
    }
    
    if (endDate.length())
    {
        cJSON_AddStringToObject(sJSON, "endDate", endDate.c_str());
    }
    
    if (message.length())
    {
        cJSON_AddStringToObject(sJSON, "payload", message.c_str());
    }
    
    if (expiry.length())
    {
        cJSON_AddStringToObject(sJSON, "expiry", expiry.c_str());
    }
    
   
    
    cJSON_AddItemReferenceToObject(messageJson, "message", sJSON);
    cJSON_AddItemReferenceToObject(pushJSON, "push", messageJson);
    cJSON_AddItemReferenceToObject(bodyJSON, "app42", pushJSON);
    
    char *cptrFormatted = cJSON_PrintUnformatted(bodyJSON);
    string bodyString = cptrFormatted;
    
    cJSON_Delete(bodyJSON);
    cJSON_Delete(pushJSON);
    cJSON_Delete(messageJson);
    cJSON_Delete(sJSON);
    
    free(cptrFormatted);
    return bodyString;
    
}


string BuildPushBody(string userName, cJSON *message,string expiry)
{
    
    cJSON *bodyJSON = cJSON_CreateObject();
    cJSON *pushJSON = cJSON_CreateObject();
    cJSON *messageJson = cJSON_CreateObject();
    cJSON *sJSON = cJSON_CreateObject();
    
    cJSON_AddStringToObject(sJSON, "userName", userName.c_str());
    //cJSON_AddStringToObject(sJSON, "payload", message.c_str());
    cJSON_AddStringToObject(sJSON, "expiry", expiry.c_str());
    cJSON_AddItemReferenceToObject(sJSON, "payload", message);
    
    cJSON_AddItemReferenceToObject(messageJson, "message", sJSON);
    cJSON_AddItemReferenceToObject(pushJSON, "push", messageJson);
    cJSON_AddItemReferenceToObject(bodyJSON, "app42", pushJSON);
    
    char *cptrFormatted = cJSON_PrintUnformatted(bodyJSON);
    string bodyString = cptrFormatted;
    
    cJSON_Delete(bodyJSON);
    cJSON_Delete(pushJSON);
    cJSON_Delete(messageJson);
    cJSON_Delete(sJSON);
    
    free(cptrFormatted);
    return bodyString;
    
}

string BuildGroupPushBody(string users, string message,string expiry)
{
    
    cJSON *bodyJSON = cJSON_CreateObject();
    cJSON *pushJSON = cJSON_CreateObject();
    cJSON *messageJson = cJSON_CreateObject();
    cJSON *sJSON = cJSON_CreateObject();
    
    cJSON_AddStringToObject(sJSON, "users", users.c_str());
    cJSON_AddStringToObject(sJSON, "payload", message.c_str());
    cJSON_AddStringToObject(sJSON, "expiry", expiry.c_str());
    
    cJSON_AddItemReferenceToObject(messageJson, "message", sJSON);
    cJSON_AddItemReferenceToObject(pushJSON, "push", messageJson);
    cJSON_AddItemReferenceToObject(bodyJSON, "app42", pushJSON);
    
    char *cptrFormatted = cJSON_PrintUnformatted(bodyJSON);
    string bodyString = cptrFormatted;
    
    cJSON_Delete(bodyJSON);
    cJSON_Delete(pushJSON);
    cJSON_Delete(messageJson);
    cJSON_Delete(sJSON);
    
    free(cptrFormatted);
    return bodyString;
    
}


string BuildSubscribeChannelBody(string userName, string channel)
{
    
    cJSON *bodyJSON = cJSON_CreateObject();
    cJSON *pushJSON = cJSON_CreateObject();
    cJSON *sJSON = cJSON_CreateObject();
    
    cJSON_AddStringToObject(sJSON, "name", channel.c_str());
    cJSON_AddStringToObject(sJSON, "username", userName.c_str());
    cJSON_AddItemReferenceToObject(pushJSON, "channel", sJSON);
    cJSON_AddItemReferenceToObject(bodyJSON, "push", pushJSON);
    
    char *cptrFormatted = cJSON_PrintUnformatted(bodyJSON);
    string bodyString = cptrFormatted;
    
    cJSON_Delete(bodyJSON);
    cJSON_Delete(pushJSON);
    cJSON_Delete(sJSON);
    
    free(cptrFormatted);
    return bodyString;
    
}

string BuildSubscribeChannelBody(string userName, string channel,string deviceToken, string deviceType)
{
    
    cJSON *bodyJSON = cJSON_CreateObject();
    cJSON *pushJSON = cJSON_CreateObject();
    cJSON *sJSON = cJSON_CreateObject();
    
    if (userName.length())
    {
        cJSON_AddStringToObject(sJSON, "userName", userName.c_str());
    }
    if (channel.length())
    {
        cJSON_AddStringToObject(sJSON, "channelName", channel.c_str());
    }
    if (deviceToken.length())
    {
        cJSON_AddStringToObject(sJSON, "deviceToken", deviceToken.c_str());
    }
    if (deviceType.length())
    {
        cJSON_AddStringToObject(sJSON, "type", deviceType.c_str());
    }
    
    cJSON_AddItemReferenceToObject(pushJSON, "push", sJSON);
    cJSON_AddItemReferenceToObject(bodyJSON, "app42", pushJSON);
    
    char *cptrFormatted = cJSON_PrintUnformatted(bodyJSON);
    string bodyString = cptrFormatted;
    
    cJSON_Delete(bodyJSON);
    cJSON_Delete(pushJSON);
    cJSON_Delete(sJSON);
    
    free(cptrFormatted);
    return bodyString;
    
}

string BuildCreateChannelBody(string channel, string description)
{
    
    cJSON *bodyJSON = cJSON_CreateObject();
    cJSON *app42JSON = cJSON_CreateObject();
    cJSON *pushJSON = cJSON_CreateObject();
    cJSON *sJSON = cJSON_CreateObject();
    
    cJSON_AddStringToObject(sJSON, "name", channel.c_str());
    cJSON_AddStringToObject(sJSON, "description", description.c_str());
    
    cJSON_AddItemReferenceToObject(pushJSON, "channel", sJSON);
    cJSON_AddItemReferenceToObject(app42JSON, "push", pushJSON);
    cJSON_AddItemReferenceToObject(bodyJSON, "app42", app42JSON);
    
    char *cptrFormatted = cJSON_PrintUnformatted(bodyJSON);
    string bodyString = cptrFormatted;
    
    cJSON_Delete(app42JSON);
    cJSON_Delete(bodyJSON);
    cJSON_Delete(pushJSON);
    cJSON_Delete(sJSON);
    
    free(cptrFormatted);
    return bodyString;
    
}


string BuildSendPushToChannelBody(string channel, string message,string expiry)
{
    
    cJSON *bodyJSON = cJSON_CreateObject();
    cJSON *pushJSON = cJSON_CreateObject();
    cJSON *messageJson = cJSON_CreateObject();
    cJSON *sJSON = cJSON_CreateObject();
    
    cJSON_AddStringToObject(sJSON, "channel", channel.c_str());
    cJSON_AddStringToObject(sJSON, "payload", message.c_str());
    cJSON_AddStringToObject(sJSON, "expiry", expiry.c_str());
    
    cJSON_AddItemReferenceToObject(messageJson, "message", sJSON);
    cJSON_AddItemReferenceToObject(pushJSON, "push", messageJson);
    cJSON_AddItemReferenceToObject(bodyJSON, "app42", pushJSON);
    
    char *cptrFormatted = cJSON_PrintUnformatted(bodyJSON);
    string bodyString = cptrFormatted;
    
    cJSON_Delete(bodyJSON);
    cJSON_Delete(pushJSON);
    cJSON_Delete(messageJson);
    cJSON_Delete(sJSON);
    
    free(cptrFormatted);
    return bodyString;
    
}

string BuildSendPushToChannelBody(string channel, cJSON *message,string expiry)
{
    
    cJSON *bodyJSON = cJSON_CreateObject();
    cJSON *pushJSON = cJSON_CreateObject();
    cJSON *messageJson = cJSON_CreateObject();
    cJSON *sJSON = cJSON_CreateObject();
    
    cJSON_AddStringToObject(sJSON, "channel", channel.c_str());
    cJSON_AddStringToObject(sJSON, "expiry", expiry.c_str());
    cJSON_AddItemReferenceToObject(sJSON, "payload", message);
    
    cJSON_AddItemReferenceToObject(messageJson, "message", sJSON);
    cJSON_AddItemReferenceToObject(pushJSON, "push", messageJson);
    cJSON_AddItemReferenceToObject(bodyJSON, "app42", pushJSON);
    
    char *cptrFormatted = cJSON_PrintUnformatted(bodyJSON);
    string bodyString = cptrFormatted;
    
    cJSON_Delete(bodyJSON);
    cJSON_Delete(pushJSON);
    cJSON_Delete(messageJson);
    cJSON_Delete(sJSON);
    
    free(cptrFormatted);
    return bodyString;
    
}

cJSON* getJsonFromMap(map<string, string>messageMap)
{
    cJSON *messageJSON = cJSON_CreateObject();
    
    for( std::map<string,string>::iterator it=messageMap.begin(); it!=messageMap.end(); ++it)
    {
       cJSON_AddStringToObject(messageJSON, it->first.c_str(), it->second.c_str());
    }
    return messageJSON;
}





void PushNotificationService::RegisterDeviceToken(const char* deviceToken, const char* userName, DeviceType deviceType, const app42CallBack& pSelector)
{
    App42PushNotificationResponse *response = new App42PushNotificationResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(userName, "User Name");
        Util::throwExceptionIfStringNullOrBlank(deviceToken, "Device Token");
        //Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    
    /**
     * Creating SignParams and signature
     */
    map<string, string> signParams;
    string timestamp = Util::getTimeStamp();
    populateSignParams(signParams);
    string pushBody = BuildRegisterDeviceTokenBody(deviceToken, userName,getDeviceType(deviceType));
    signParams["body"] = pushBody;
    string signature = Util::signMap(secretKey, signParams);
    
    /**
     * Creating URL
     */
    string resource = "push/storeDeviceToken/";
	resource.append(userName);
    string baseUrl = getBaseUrl(resource);
    baseUrl.append("?");
    string encodedUrl = url_encode(baseUrl);
    /**
     * Creating Headers
     */
    std::vector<std::string> headers;
    map<string, string> metaHeaders;
    populateMetaHeaderParams(metaHeaders);
    Util::BuildHeaders(metaHeaders, headers);
    Util::BuildHeaders(apiKey, timestamp, VERSION, signature, headers);
    
    /**
     * Initiating Http call
     */
    Util::executePost(encodedUrl, headers, pushBody.c_str(), std::bind(&App42PushNotificationResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
    
}

void PushNotificationService::SendPushMessageToUser(const char* username,  const char* message, const app42CallBack& pSelector)
{
    App42PushNotificationResponse *response = new App42PushNotificationResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(username, "User Name");
        Util::throwExceptionIfStringNullOrBlank(message, "Message");
        //Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    /**
     * Creating SignParams and signature
     */
    map<string, string> signParams;
    string timestamp = Util::getTimeStamp();
    populateSignParams(signParams);
    string pushBody = BuildPushBody(username,message,timestamp,"");
    signParams["body"] = pushBody;
    string signature = Util::signMap(secretKey, signParams);
    
    /**
     * Creating URL
     */
    string resource = "push/sendMessage/";
	resource.append(username);
    string baseUrl = getBaseUrl(resource);
    baseUrl.append("?");
    string encodedUrl = url_encode(baseUrl);
    /**
     * Creating Headers
     */
    std::vector<std::string> headers;
    map<string, string> metaHeaders;
    populateMetaHeaderParams(metaHeaders);
    Util::BuildHeaders(metaHeaders, headers);
    Util::BuildHeaders(apiKey, timestamp, VERSION, signature, headers);
    
    /**
     * Initiating Http call
     */
    Util::executePost(encodedUrl, headers, pushBody.c_str(), std::bind(&App42PushNotificationResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
}

void PushNotificationService::SendPushMessageToUser(const char* username, map<string, string>messageMap, const app42CallBack& pSelector)
{
    App42PushNotificationResponse *response = new App42PushNotificationResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(username, "User Name");
        //Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
        Util::throwExceptionIfCallBackIsNull(pSelector, "Callback");
        Util::throwExceptionIfMapIsNullOrBlank(messageMap, "Push Message");
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
    /**
     * Creating SignParams and signature
     */
    
    map<string, string> signParams;
    string timestamp = Util::getTimeStamp();
    populateSignParams(signParams);
    cJSON *message = getJsonFromMap(messageMap);
    string pushBody = BuildPushBody(username,message,timestamp);
    signParams["body"] = pushBody;
    string signature = Util::signMap(secretKey, signParams);
   
    /**
     * Creating URL
     */
    string resource = "push/sendMessage/";
	resource.append(username);
    string baseUrl = getBaseUrl(resource);
    baseUrl.append("?");
    string encodedUrl = url_encode(baseUrl);
    /**
     * Creating Headers
     */
    std::vector<std::string> headers;
    map<string, string> metaHeaders;
    populateMetaHeaderParams(metaHeaders);
    Util::BuildHeaders(metaHeaders, headers);
    Util::BuildHeaders(apiKey, timestamp, VERSION, signature, headers);
    
    /**
     * Initiating Http call
     */
    Util::executePost(encodedUrl, headers, pushBody.c_str(), std::bind(&App42PushNotificationResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
}

void PushNotificationService::SendPushMessageToAllByType(const char* message,DeviceType deviceType, const app42CallBack& pSelector)
{
    App42PushNotificationResponse *response = new App42PushNotificationResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(message, "Message");
        //Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    /**
     * Creating SignParams and signature
     */
    map<string, string> signParams;
    string timestamp = Util::getTimeStamp();
    populateSignParams(signParams);
    string pushBody = BuildPushBody("",message,timestamp,getDeviceType(deviceType));
    signParams["body"] = pushBody;
    string signature = Util::signMap(secretKey, signParams);
    
    /**
     * Creating URL
     */
    string resource = "push/sendMessageToAllByType";
    string baseUrl = getBaseUrl(resource);
    baseUrl.append("?");
    string encodedUrl = url_encode(baseUrl);
    /**
     * Creating Headers
     */
    std::vector<std::string> headers;
    map<string, string> metaHeaders;
    populateMetaHeaderParams(metaHeaders);
    Util::BuildHeaders(metaHeaders, headers);
    Util::BuildHeaders(apiKey, timestamp, VERSION, signature, headers);
    
    /**
     * Initiating Http call
     */
    Util::executePost(encodedUrl, headers, pushBody.c_str(), std::bind(&App42PushNotificationResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
}

void PushNotificationService::SendPushMessageToAll(const char* message, const app42CallBack& pSelector)
{
    App42PushNotificationResponse *response = new App42PushNotificationResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(message, "Message");
        //Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    /**
     * Creating SignParams and signature
     */
    map<string, string> signParams;
    string timestamp = Util::getTimeStamp();
    populateSignParams(signParams);
    string pushBody = BuildPushBody("",message,timestamp,"");
    signParams["body"] = pushBody;
    string signature = Util::signMap(secretKey, signParams);
    
    /**
     * Creating URL
     */
    string resource = "push/sendPushMessageToAll";
    string baseUrl = getBaseUrl(resource);
    baseUrl.append("?");
    string encodedUrl = url_encode(baseUrl);
    /**
     * Creating Headers
     */
    std::vector<std::string> headers;
    map<string, string> metaHeaders;
    populateMetaHeaderParams(metaHeaders);
    Util::BuildHeaders(metaHeaders, headers);
    Util::BuildHeaders(apiKey, timestamp, VERSION, signature, headers);
    
    /**
     * Initiating Http call
     */
    Util::executePost(encodedUrl, headers, pushBody.c_str(), std::bind(&App42PushNotificationResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
}

void PushNotificationService::SubscribeToChannel(const char* channel, const char* userName, const app42CallBack& pSelector)
{
    App42PushNotificationResponse *response = new App42PushNotificationResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(userName, "User Name");
        Util::throwExceptionIfStringNullOrBlank(channel, "Channel");
        //Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    /**
     * Creating SignParams and signature
     */
    map<string, string> signParams;
    string timestamp = Util::getTimeStamp();
    populateSignParams(signParams);
    string pushBody = BuildSubscribeChannelBody(userName,channel);
    signParams["body"] = pushBody;
    string signature = Util::signMap(secretKey, signParams);
    
    
//    Util::printMap(signParams);
    /**
     * Creating URL
     */
    string resource = "push/subscribeToChannel/";
	resource.append(userName);
    string baseUrl = getBaseUrl(resource);
    baseUrl.append("?");
    string encodedUrl = url_encode(baseUrl);

    /**
     * Creating Headers
     */
    std::vector<std::string> headers;
    map<string, string> metaHeaders;
    populateMetaHeaderParams(metaHeaders);
    Util::BuildHeaders(metaHeaders, headers);
    Util::BuildHeaders(apiKey, timestamp, VERSION, signature, headers);
    
    /**
     * Initiating Http call
     */
    Util::executePost(encodedUrl, headers, pushBody.c_str(), std::bind(&App42PushNotificationResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
    
}

void PushNotificationService::SubscribeToChannel(const char* channel,const char* userName, const char* deviceToken, DeviceType deviceType, const app42CallBack& pSelector)
{
    App42PushNotificationResponse *response = new App42PushNotificationResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(userName, "User Name");
        Util::throwExceptionIfStringNullOrBlank(channel, "Channel");
        Util::throwExceptionIfStringNullOrBlank(deviceToken, "Device Token");
        //Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    /**
     * Creating SignParams and signature
     */
    map<string, string> signParams;
    string timestamp = Util::getTimeStamp();
    populateSignParams(signParams);
    string pushBody = BuildSubscribeChannelBody(userName,channel,deviceToken,getDeviceType(deviceType));
    signParams["body"] = pushBody;
    string signature = Util::signMap(secretKey, signParams);
    
    
    /**
     * Creating URL
     */
    string resource = "push/subscribeDeviceToChannel";
    string baseUrl = getBaseUrl(resource);
    baseUrl.append("?");
    string encodedUrl = url_encode(baseUrl);
    /**
     * Creating Headers
     */
    std::vector<std::string> headers;
    map<string, string> metaHeaders;
    populateMetaHeaderParams(metaHeaders);
    Util::BuildHeaders(metaHeaders, headers);
    Util::BuildHeaders(apiKey, timestamp, VERSION, signature, headers);
    
    /**
     * Initiating Http call
     */
    Util::executePost(encodedUrl, headers, pushBody.c_str(), std::bind(&App42PushNotificationResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
    
}
void PushNotificationService::UnsubscribeFromChannel(const char* channel, const char* userName, const app42CallBack& pSelector)
{
    App42PushNotificationResponse *response = new App42PushNotificationResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(userName, "User Name");
        Util::throwExceptionIfStringNullOrBlank(channel, "Channel");
        //Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    /**
     * Creating SignParams and signature
     */
    map<string, string> signParams;
    string timestamp = Util::getTimeStamp();
    populateSignParams(signParams);
    string pushBody = BuildSubscribeChannelBody(userName,channel);
    signParams["body"] = pushBody;
    string signature = Util::signMap(secretKey, signParams);
    
    /**
     * Creating URL
     */
    string resource = "push/unsubscribeToChannel/";
	resource.append(userName);
    string baseUrl = getBaseUrl(resource);
    baseUrl.append("?");
    string encodedUrl = url_encode(baseUrl);

    /**
     * Creating Headers
     */
    std::vector<std::string> headers;
    map<string, string> metaHeaders;
    populateMetaHeaderParams(metaHeaders);
    Util::BuildHeaders(metaHeaders, headers);
    Util::BuildHeaders(apiKey, timestamp, VERSION, signature, headers);
    
    /**
     * Initiating Http call
     */
    Util::executePut(encodedUrl, headers, pushBody.c_str(), std::bind(&App42PushNotificationResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
    
}

void PushNotificationService::UnsubscribeDeviceToChannel(const char* channel,const char* userName, const char* deviceToken, const app42CallBack& pSelector)
{
    App42PushNotificationResponse *response = new App42PushNotificationResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(userName, "User Name");
        Util::throwExceptionIfStringNullOrBlank(channel, "Channel");
        Util::throwExceptionIfStringNullOrBlank(deviceToken, "Device Token");
        //Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    
    /**
     * Creating SignParams and signature
     */
    map<string, string> signParams;
    string timestamp = Util::getTimeStamp();
    populateSignParams(signParams);
    string pushBody = BuildSubscribeChannelBody(userName,channel,deviceToken,"");
    signParams["body"] = pushBody;
    string signature = Util::signMap(secretKey, signParams);
    
    /**
     * Creating URL
     */
    string resource = "push/unsubscribeDeviceToChannel";
    string baseUrl = getBaseUrl(resource);
    baseUrl.append("?");
    string encodedUrl = url_encode(baseUrl);
    
    /**
     * Creating Headers
     */
    std::vector<std::string> headers;
    map<string, string> metaHeaders;
    populateMetaHeaderParams(metaHeaders);
    Util::BuildHeaders(metaHeaders, headers);
    Util::BuildHeaders(apiKey, timestamp, VERSION, signature, headers);
    
    /**
     * Initiating Http call
     */
    Util::executePost(encodedUrl, headers, pushBody.c_str(), std::bind(&App42PushNotificationResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
    
}

void PushNotificationService::SendPushMessageToChannel(const char* channel, const char* message, const app42CallBack& pSelector)
{
    App42PushNotificationResponse *response = new App42PushNotificationResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(channel, "Channel Name");
        Util::throwExceptionIfStringNullOrBlank(message, "Message");
        //Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    /**
     * Creating SignParams and signature
     */
    map<string, string> signParams;
    string timestamp = Util::getTimeStamp();
    populateSignParams(signParams);
    string pushBody = BuildSendPushToChannelBody(channel, message,timestamp);
    signParams["body"] = pushBody;
    string signature = Util::signMap(secretKey, signParams);
    
    /**
     * Creating URL
     */
    string resource = "push/sendPushMessageToChannel/";
	resource.append(channel);
    string baseUrl = getBaseUrl(resource);
    baseUrl.append("?");
    string encodedUrl = url_encode(baseUrl);
   
    /**
     * Creating Headers
     */
    std::vector<std::string> headers;
    map<string, string> metaHeaders;
    populateMetaHeaderParams(metaHeaders);
    Util::BuildHeaders(metaHeaders, headers);
    Util::BuildHeaders(apiKey, timestamp, VERSION, signature, headers);
   
    /**
     * Initiating Http call
     */
    Util::executePost(encodedUrl, headers, pushBody.c_str(), std::bind(&App42PushNotificationResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
}

void PushNotificationService::SendPushMessageToChannel(const char* channel, map<string, string>messageMap, const app42CallBack& pSelector)
{
    App42PushNotificationResponse *response = new App42PushNotificationResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(channel, "Channel Name");
        Util::throwExceptionIfMapIsNullOrBlank(messageMap, "Push Message");
        //Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    
    /**
     * Creating SignParams and signature
     */
    map<string, string> signParams;
    string timestamp = Util::getTimeStamp();
    populateSignParams(signParams);
    cJSON *message = getJsonFromMap(messageMap);
    string pushBody = BuildSendPushToChannelBody(channel, message, timestamp);
    signParams["body"] = pushBody;
    
    string signature = Util::signMap(secretKey, signParams);
    
    /**
     * Creating URL
     */
    string resource = "push/sendPushMessageToChannel/";
	resource.append(channel);
    string baseUrl = getBaseUrl(resource);
    baseUrl.append("?");
    string encodedUrl = url_encode(baseUrl);
    /**
     * Creating Headers
     */
    std::vector<std::string> headers;
    map<string, string> metaHeaders;
    populateMetaHeaderParams(metaHeaders);
    Util::BuildHeaders(metaHeaders, headers);
    Util::BuildHeaders(apiKey, timestamp, VERSION, signature, headers);
    
    /**
     * Initiating Http call
     */
    Util::executePost(encodedUrl, headers, pushBody.c_str(), std::bind(&App42PushNotificationResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
}

void PushNotificationService::CreateChannel(const char* channel,const char* description,const app42CallBack& pSelector)
{
    App42PushNotificationResponse *response = new App42PushNotificationResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(channel, "Channel Name");
        Util::throwExceptionIfStringNullOrBlank(description, "Description");
        //Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    
    /**
     * Creating SignParams and signature
     */
    map<string, string> signParams;
    string timestamp = Util::getTimeStamp();
    populateSignParams(signParams);
    string pushBody = BuildCreateChannelBody(channel, description);
    signParams["body"] = pushBody;
    string signature = Util::signMap(secretKey, signParams);
    
    /**
     * Creating URL
     */
    string resource = "push/createAppChannel";
    string baseUrl = getBaseUrl(resource);
    baseUrl.append("?");
    string encodedUrl = url_encode(baseUrl);
    /**
     * Creating Headers
     */
    std::vector<std::string> headers;
    map<string, string> metaHeaders;
    populateMetaHeaderParams(metaHeaders);
    Util::BuildHeaders(metaHeaders, headers);
    Util::BuildHeaders(apiKey, timestamp, VERSION, signature, headers);
    
    /**
     * Initiating Http call
     */
    Util::executePost(encodedUrl, headers, pushBody.c_str(), std::bind(&App42PushNotificationResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
    
}

void PushNotificationService::DeleteDeviceToken(const char* deviceToken, const char* userName, const app42CallBack& pSelector)
{
    App42PushNotificationResponse *response = new App42PushNotificationResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(userName, "User Name");
        Util::throwExceptionIfStringNullOrBlank(deviceToken, "Device Token");
        //Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    
    /**
     * Creating SignParams and signature
     */
    map<string, string> signParams;
    string timestamp = Util::getTimeStamp();
    populateSignParams(signParams);
    signParams["userName"] = userName;
    signParams["deviceToken"] = deviceToken;
    string signature = Util::signMap(secretKey, signParams);
    
    /**
     * Creating URL
     */
    string resource = "push";
    string baseUrl = getBaseUrl(resource);
    baseUrl.append("?");
    
    /**
     * Creating Query Params
     */
    map<string, string> queryParams;
    queryParams["userName"] = userName;
    queryParams["deviceToken"] = deviceToken;
    string queryString = buildQueryString(queryParams);
    baseUrl.append(queryString);
    string encodedUrl = url_encode(baseUrl);
    /**
     * Creating Headers
     */
    std::vector<std::string> headers;
    map<string, string> metaHeaders;
    populateMetaHeaderParams(metaHeaders);
    Util::BuildHeaders(metaHeaders, headers);
    Util::BuildHeaders(apiKey, timestamp, VERSION, signature, headers);
    
    /**
     * Initiating Http call
     */
    Util::executeDelete(encodedUrl, headers, std::bind(&App42PushNotificationResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
    
}

void PushNotificationService::DeleteAllDevices(const char* userName, const app42CallBack& pSelector)
{
    App42PushNotificationResponse *response = new App42PushNotificationResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(userName, "User Name");
        //Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    
    /**
     * Creating SignParams and signature
     */
    map<string, string> signParams;
    string timestamp = Util::getTimeStamp();
    populateSignParams(signParams);
    signParams["userName"] = userName;
    string signature = Util::signMap(secretKey, signParams);
    
    /**
     * Creating URL
     */
    string resource = "push/deleteAll";
    string baseUrl = getBaseUrl(resource);
    baseUrl.append("?");
    
    /**
     * Creating Query Params
     */
    map<string, string> queryParams;
    queryParams["userName"] = userName;
    string queryString = buildQueryString(queryParams);
    baseUrl.append(queryString);
    string encodedUrl = url_encode(baseUrl);
    /**
     * Creating Headers
     */
    std::vector<std::string> headers;
    map<string, string> metaHeaders;
    populateMetaHeaderParams(metaHeaders);
    Util::BuildHeaders(metaHeaders, headers);
    Util::BuildHeaders(apiKey, timestamp, VERSION, signature, headers);
    
    /**
     * Initiating Http call
     */
    Util::executeDelete(encodedUrl, headers, std::bind(&App42PushNotificationResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
    
}

void PushNotificationService::UnsubscribeDevice(const char* deviceToken, const char* userName, const app42CallBack& pSelector)
{
    App42PushNotificationResponse *response = new App42PushNotificationResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(userName, "User Name");
        Util::throwExceptionIfStringNullOrBlank(deviceToken, "Device Token");
        //Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    
    string timestamp = Util::getTimeStamp();

    /**
     * Creating SignParams and signature
     */
    map<string, string> signParams;
    populateSignParams(signParams);
    string pushBody = BuildUnsubcribeDeviceBody(deviceToken, userName);
    signParams["body"] = pushBody;
    string signature = Util::signMap(secretKey, signParams);
    //Util::printMap(signParams);
    /**
     * Creating URL
     */
    string resource = "push/unsubscribeDevice";
    string baseUrl = getBaseUrl(resource);
    baseUrl.append("?");
    string encodedUrl = url_encode(baseUrl);
    /**
     * Creating Headers
     */
    std::vector<std::string> headers;
    map<string, string> metaHeaders;
    populateMetaHeaderParams(metaHeaders);
    Util::BuildHeaders(metaHeaders, headers);
    Util::BuildHeaders(apiKey, timestamp, VERSION, signature, headers);
    
    /**
     * Initiating Http call
     */
    Util::executePut(encodedUrl, headers, pushBody.c_str(), std::bind(&App42PushNotificationResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
    
}

void PushNotificationService::ResubscribeDevice(const char* deviceToken, const char* userName, const app42CallBack& pSelector)
{
    App42PushNotificationResponse *response = new App42PushNotificationResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(userName, "User Name");
        Util::throwExceptionIfStringNullOrBlank(deviceToken, "Device Token");
        //Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    
    /**
     * Creating SignParams and signature
     */
    map<string, string> signParams;
    string timestamp = Util::getTimeStamp();
    populateSignParams(signParams);
    string pushBody = BuildUnsubcribeDeviceBody(deviceToken, userName);
    signParams["body"] = pushBody;
    string signature = Util::signMap(secretKey, signParams);
    
    /**
     * Creating URL
     */
    string resource = "push/reSubscribeDevice";
    string baseUrl = getBaseUrl(resource);
    baseUrl.append("?");
    string encodedUrl = url_encode(baseUrl);
    /**
     * Creating Headers
     */
    std::vector<std::string> headers;
    map<string, string> metaHeaders;
    populateMetaHeaderParams(metaHeaders);
    Util::BuildHeaders(metaHeaders, headers);
    Util::BuildHeaders(apiKey, timestamp, VERSION, signature, headers);
    
    /**
     * Initiating Http call
     */
    Util::executePut(encodedUrl, headers, pushBody.c_str(), std::bind(&App42PushNotificationResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
    
}

void PushNotificationService::SendPushMessageToGroup(vector<string> userList,const char* message, const app42CallBack& pSelector)
{
    App42PushNotificationResponse *response = new App42PushNotificationResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(message, "Message");
        //Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    /**
     * Creating SignParams and signature
     */
    map<string, string> signParams;
    string timestamp = Util::getTimeStamp();
    populateSignParams(signParams);
    string groupString = getJsonStringFromUserList(userList);
    
    string pushBody = BuildGroupPushBody(groupString,message,timestamp);
    signParams["body"] = pushBody;
    string signature = Util::signMap(secretKey, signParams);

    /**
     * Creating URL
     */
    string resource = "push/sendPushMessageToGroup";
    string baseUrl = getBaseUrl(resource);
    baseUrl.append("?");
    string encodedUrl = url_encode(baseUrl);
    /**
     * Creating Headers
     */
    std::vector<std::string> headers;
    map<string, string> metaHeaders;
    populateMetaHeaderParams(metaHeaders);
    Util::BuildHeaders(metaHeaders, headers);
    Util::BuildHeaders(apiKey, timestamp, VERSION, signature, headers);
    
    /**
     * Initiating Http call
     */
    Util::executePost(encodedUrl, headers, pushBody.c_str(), std::bind(&App42PushNotificationResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
}

void PushNotificationService::SendPushToTargetUsers(const char* message,const char* dbName, const char* collectionName, Query *query, const app42CallBack& pSelector)
{
    App42PushNotificationResponse *response = new App42PushNotificationResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(message, "Message");
        Util::throwExceptionIfStringNullOrBlank(dbName, "DB Name");
        Util::throwExceptionIfStringNullOrBlank(collectionName, "Collection Name");
        Util::throwExceptionIfObjectIsNull(query, "Query");
        //Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    /**
     * Creating SignParams and signature
     */
    map<string, string> signParams;
    string timestamp = Util::getTimeStamp();
    populateSignParams(signParams);
    string pushBody = BuildPushBody("",message,timestamp,"");
    signParams["body"] = pushBody;
    signParams["jsonQuery"] = query->getString();
    string signature = Util::signMap(secretKey, signParams);
    Util::printMap(signParams);

    /**
     * Creating URL
     */
    string resource = "push/sendTargetPush/";
	resource.append(dbName);
    resource.append("/");

    resource.append(collectionName);
    string baseUrl = getBaseUrl(resource);
    baseUrl.append("?");
    
    /**
     * Creating Query Params
     */
    map<string, string> queryParams;
    queryParams["jsonQuery"] = query->getString();
    string queryString = buildQueryString(queryParams);
    baseUrl.append(queryString);
    string encodedUrl = url_encode(baseUrl);
    
    /**
     * Creating Headers
     */
    std::vector<std::string> headers;
    map<string, string> metaHeaders;
    populateMetaHeaderParams(metaHeaders);
    Util::BuildHeaders(metaHeaders, headers);
    Util::BuildHeaders(apiKey, timestamp, VERSION, signature, headers);
    
    /**
     * Initiating Http call
     */
    Util::executePost(encodedUrl, headers, pushBody.c_str(), std::bind(&App42PushNotificationResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
}



void PushNotificationService::ScheduleMessageToUser(const char* userName,const char* message, tm *expiryDate, const app42CallBack& pSelector)
{
    App42PushNotificationResponse *response = new App42PushNotificationResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(message, "Message");
        //Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
        Util::throwExceptionIfCallBackIsNull(pSelector, "Callback");
    }
    catch (App42Exception *e)
    {
        std::string ex = e->what();
        response->httpErrorCode = e->getHttpErrorCode();
        response->appErrorCode  = e->getAppErrorCode();
        response->errorDetails  = ex;
        response->isSuccess = false;
        if (pSelector)
        {
            pSelector(response); 
        }
        delete e;
        e = NULL;
        return;
    }
    
    /**
     * Creating SignParams and signature
     */
    map<string, string> signParams;
    string timestamp = Util::getTimeStamp();
    populateSignParams(signParams);
    
    string strExpiryDate = Util::getTimeStamp(expiryDate);
    
    string pushBody = BuildPushBody(userName, message, strExpiryDate, "");
    signParams["body"] = pushBody;
    string signature = Util::signMap(secretKey, signParams);
    
    Util::printMap(signParams);
    
    /**
     * Creating URL
     */
    string resource = "push/sendMessage/";
    resource.append(userName);
    string baseUrl = getBaseUrl(resource);
    baseUrl.append("?");
    string encodedUrl = url_encode(baseUrl);
    
    /**
     * Creating Headers
     */
    std::vector<std::string> headers;
    map<string, string> metaHeaders;
    populateMetaHeaderParams(metaHeaders);
    Util::BuildHeaders(metaHeaders, headers);
    Util::BuildHeaders(apiKey, timestamp, VERSION, signature, headers);
    
    /**
     * Initiating Http call
     */
    Util::executePost(encodedUrl, headers, pushBody.c_str(), std::bind(&App42PushNotificationResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
}

void PushNotificationService::SendPushMessageToDevice(const char* deviceToken, const char* username,  const char* message, const app42CallBack& pSelector)
{
    App42PushNotificationResponse *response = new App42PushNotificationResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(deviceToken, "Device Token");
        Util::throwExceptionIfStringNullOrBlank(username, "User Name");
        Util::throwExceptionIfStringNullOrBlank(message, "Message");
        //Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    /**
     * Creating SignParams and signature
     */
    map<string, string> signParams;
    string timestamp = Util::getTimeStamp();
    populateSignParams(signParams);
    string pushBody = BuildPushBody(username,message,timestamp,"",deviceToken);
    signParams["body"] = pushBody;
    string signature = Util::signMap(secretKey, signParams);
    
    /**
     * Creating URL
     */
    string resource = "push/sendMessageToDevice/";
	resource.append(username);
    string baseUrl = getBaseUrl(resource);
    baseUrl.append("?");
    string encodedUrl = url_encode(baseUrl);
    /**
     * Creating Headers
     */
    std::vector<std::string> headers;
    map<string, string> metaHeaders;
    populateMetaHeaderParams(metaHeaders);
    Util::BuildHeaders(metaHeaders, headers);
    Util::BuildHeaders(apiKey, timestamp, VERSION, signature, headers);
    
    /**
     * Initiating Http call
     */
    Util::executePost(encodedUrl, headers, pushBody.c_str(), std::bind(&App42PushNotificationResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
}
