//
//  App42Service.cpp
//  App42CPPSDK
//
//  Created by Rajeev Ranjan on 07/10/13.
//
//

#include "App42Service.h"
#include "cJSON.h"
#include "App42Constants.h"
#include "Signing.h"
#include <sstream>
#include "App42API.h"
#include "Connector.h"


App42Service::App42Service()
{
    pageMaxRecords = -1;
    pageOffset = -1;
    fbAccessToken = "";
}

App42Service::~App42Service()
{
    
}


void App42Service::Init(string _apikey, string _secretkey)
{
    apiKey = _apikey;
    secretKey = _secretkey;
    fbAccessToken = "";
}

const char* App42Service::getJsonStringFromVector(const char* key,vector<string>userList)
{
    cJSON *bodyJSON = cJSON_CreateObject();
    cJSON *userJSON = cJSON_CreateArray();
    
    std::vector<string>::iterator it;
    
    for(it=userList.begin(); it!=userList.end(); ++it)
    {
        cJSON *jsonString = cJSON_CreateString(it->c_str());
        cJSON_AddItemToArray(userJSON, jsonString);
        //cJSON_Delete(jsonString);
    }
    cJSON_AddItemReferenceToObject(bodyJSON, key, userJSON);
    
    char *cptrFormatted = cJSON_PrintUnformatted(bodyJSON);
    string bodyString = cptrFormatted;
    
    cJSON_Delete(userJSON);
    cJSON_Delete(bodyJSON);
    
    free(cptrFormatted);
    
    return bodyString.c_str();
    
}

const char* App42Service::getJsonStringFromVector(vector<string>array)
{
    cJSON *userJSON = cJSON_CreateArray();
    
    std::vector<string>::iterator it;
    
    for(it=array.begin(); it!=array.end(); ++it)
    {
        cJSON *jsonString = cJSON_CreateString(it->c_str());
        cJSON_AddItemToArray(userJSON, jsonString);
        //cJSON_Delete(jsonString);
    }
    
    char *cptrFormatted = cJSON_PrintUnformatted(userJSON);
    string bodyString = cptrFormatted;
    
    cJSON_Delete(userJSON);
    
    free(cptrFormatted);
    
    return bodyString.c_str();
}



void App42Service::populateSignParams(std::map<string, string>& signParamsMap)
{
    signParamsMap["apiKey"] = apiKey;
    signParamsMap["timeStamp"] = Util::getTimeStamp();
    signParamsMap["version"] = VERSION;
    
    if (adminKey.length()>0)
    {
        signParamsMap["adminKey"] = adminKey;
    }
    
    if (sessionId.length()>0)
    {
        signParamsMap["sessionId"] = sessionId;
    }
    else
    {
        string _sessionId = App42API::getUserSessionId();
        if (_sessionId.length())
        {
            signParamsMap["sessionId"] = _sessionId;
        }
    }
    
    if (fbAccessToken.length()>0)
    {
        signParamsMap["fbAccessToken"] = fbAccessToken;
    }
    else
    {
        string accessToken = App42API::getFbAccesToken();
        if (accessToken.length())
        {
            signParamsMap["fbAccessToken"] = accessToken;
        }
    }
}

void App42Service::populateMetaHeaderParams(std::map<string, string>& metaHeaderParamsMap)
{
    if (pageMaxRecords!=-1)
    {
        std::stringstream ss;
        ss << pageMaxRecords;
        metaHeaderParamsMap[PAGE_MAX_RECORDS] = ss.str();
    }
    
    if (pageOffset!=-1)
    {
        std::stringstream ss;
        ss << pageOffset;
        metaHeaderParamsMap[PAGE_OFFSET] = ss.str();
    }
    if (adminKey.length()>0)
    {
        metaHeaderParamsMap["adminKey"] = adminKey;
    }
    setApp42ACLHeader(metaHeaderParamsMap);
    
    if((geoTag.c_str()!=NULL) && geoTag.length())
    {
        metaHeaderParamsMap[GEO_TAG] = geoTag;
    }
    
    if ((event.c_str()!=NULL) && event.length())
    {
        metaHeaderParamsMap[EVENT] = event;
    }
    
    string loggedInUser = App42API::getLoggedInUser();
    if ((loggedInUser.c_str()!=NULL) && loggedInUser.length())
    {
        metaHeaderParamsMap["loggedInUser"] = loggedInUser;
    }
    
    string installationID = App42API::getInstallId();
    if ((installationID.c_str()!=NULL) && installationID.length())
    {
        metaHeaderParamsMap["deviceId"] = installationID;
    }
    
    if ((query.c_str()!=NULL) && query.length())
    {
        metaHeaderParamsMap["metaQuery"] = query;
    }
    
    if ((jsonObject.c_str()!=NULL) && jsonObject.length())
    {
        metaHeaderParamsMap["jsonObject"] = jsonObject;
    }
    
    if ((dbName.c_str()!=NULL) && dbName.length() && (collectionName.c_str()!=NULL) && collectionName.length())
    {
        cJSON *obj = cJSON_CreateObject();
        cJSON_AddStringToObject(obj, "dbName", dbName.c_str());
        cJSON_AddStringToObject(obj, "collectionName", collectionName.c_str());
        metaHeaderParamsMap["dbCredentials"] = cJSON_PrintUnformatted(obj);
    }
    
    std::map<string,string>::iterator it;
    
    for(it=otherMetaHearders.begin(); it!=otherMetaHearders.end(); ++it)
    {
        metaHeaderParamsMap[it->first] = it->second;
    }
    
    if (sessionId.length()>0)
    {
        metaHeaderParamsMap["sessionId"] = sessionId;
    }
    else
    {
        string _sessionId = App42API::getUserSessionId();
        if (_sessionId.length())
        {
            metaHeaderParamsMap["sessionId"] = _sessionId;
        }
    }
    
    if (fbAccessToken.length()>0)
    {
        metaHeaderParamsMap["fbAccessToken"] = fbAccessToken;
    }
    else
    {
        string accessToken = App42API::getFbAccesToken();
        if (accessToken.length())
        {
            metaHeaderParamsMap["fbAccessToken"] = accessToken;
        }
    }
    
    printf("metaHeaderParamsMap: ");
    //Printing Headers
    Util::printMap(metaHeaderParamsMap);
    
}

void App42Service::setApp42ACLHeader(std::map<string, string>& metaHeaderParamsMap)
{
    if (App42ACLList.size()>0)
    {
        metaHeaderParamsMap["dataApp42ACL"] = getJsonStringFromApp42ACLList(App42ACLList);
    }
    else if(App42API::getDefaultApp42ACL().size()>0)
    {
        vector<App42ACL> _App42ACLList = App42API::getDefaultApp42ACL();
        metaHeaderParamsMap["dataApp42ACL"] = getJsonStringFromApp42ACLList(_App42ACLList);
    }
}

string App42Service::getJsonStringFromApp42ACLList(vector<App42ACL> &App42ACLObjectArray)
{
    cJSON *jsonArray = cJSON_CreateArray();
    
    for (vector<App42ACL>::iterator it = App42ACLObjectArray.begin(); it!= App42ACLObjectArray.end(); it++)
    {
        cJSON *jsonObject = cJSON_CreateObject();
        cJSON_AddStringToObject(jsonObject, it->getUserName().c_str(), it->getPermission().c_str());
        cJSON_AddItemToArray(jsonArray, jsonObject);
        //cJSON_Delete(jsonObject);
    }
    
    char *cptrFormatted = cJSON_PrintUnformatted(jsonArray);
    string jsonString = cptrFormatted;
    free(cptrFormatted);
    cJSON_Delete(jsonArray);
    return jsonString;
}

string App42Service::getJsonStringFromApp42ACLList(string key,vector<App42ACL> &App42ACLObjectArray)
{
    cJSON *bodyJSON = cJSON_CreateObject();
    cJSON *jsonArray = cJSON_CreateArray();
    
    for (vector<App42ACL>::iterator it = App42ACLObjectArray.begin(); it!= App42ACLObjectArray.end(); it++)
    {
        cJSON *jsonObject = cJSON_CreateObject();
        cJSON_AddStringToObject(jsonObject, "user", it->getUserName().c_str());
        cJSON_AddStringToObject(jsonObject, "permission", it->getPermission().c_str());
        cJSON_AddItemToArray(jsonArray, jsonObject);
        //cJSON_Delete(jsonObject);
    }
    cJSON_AddItemReferenceToObject(bodyJSON, key.c_str(), jsonArray);

    char *cptrFormatted = cJSON_PrintUnformatted(bodyJSON);
    string jsonString = cptrFormatted;
    free(cptrFormatted);
    cJSON_Delete(jsonArray);
    cJSON_Delete(bodyJSON);
    return jsonString;
}

string App42Service::getBaseUrl(string resource)
{
    string url = "";
    url.append(PROTOCOL);
    url.append(BASE_URL);
    url.append(RESOURCE_BASE);
    url.append(resource);
    return url;
}

string App42Service::getAnalyticsUrl(string resource)
{
    string url = "";
    url.append(PROTOCOL);
    url.append(APP42_ANALYTICS_URL);
    url.append(RESOURCE_BASE);
    url.append(resource);
    return url;
}


string App42Service::getCustomCodeUrl(string resource)
{
    string url = "";
    url.append(PROTOCOL);
    url.append(CUSTOMCODE_URL);
    url.append(VERSION);
    url.append("/" + resource);
    return url;
}

string App42Service::buildQueryString(map<string, string> queryParamsMap)
{
    string queryString = "";
    
    std::map<string,string>::iterator it;
    
    for(it=queryParamsMap.begin(); it!=queryParamsMap.end(); ++it)
    {
        queryString.append(it->first);
        queryString.append("=");
        queryString.append(it->second);
        queryString.append("&");
    }
    return queryString;
}

void App42Service::setQuery(string _collectionName,Query *_metaInfoQuery)
{
    string _dbName = App42API::getDbName();
    if (_dbName.length())
    {
        dbName = _dbName;
    }
    if (_collectionName.length())
    {
        collectionName = _collectionName;
    }
    if (_metaInfoQuery != NULL)
    {
        query = _metaInfoQuery->getString();
    }
}

void App42Service::setQuery(string _dbName, string _collectionName,Query *_metaInfoQuery)
{
    dbName = _dbName;
    collectionName = _collectionName;
    query = _metaInfoQuery->getString();
}

void App42Service::setQuery(string _dbName, string _collectionName)
{
    dbName = _dbName;
    collectionName = _collectionName;
}

void App42Service::setOtherMetaHeaders(std::map<std::string, std::string> l_otherMetaHearders)
{
    otherMetaHearders = l_otherMetaHearders;
}

std::string App42Service::getFbAccessToken()
{
    return fbAccessToken;
}

void App42Service::setFbAccessToken(std::string accessToken)
{
    fbAccessToken = accessToken;
}

std::string App42Service::getAdminKey()
{
    return adminKey;
}
void App42Service::setAdminKey(std::string _adminKey)
{
    adminKey = _adminKey;
}

void App42Service::setSessionId(std::string _sessionId)
{
    sessionId = _sessionId;
}
