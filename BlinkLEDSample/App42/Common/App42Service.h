//
//  App42Service.h
//  App42CPPSDK
//
//  Created by Rajeev Ranjan on 07/10/13.
//
//

#ifndef __App42CPPSDK__App42Service__
#define __App42CPPSDK__App42Service__

#include <iostream>
#include <map>
#include <vector>
#include "Query.h"
#include "App42CallBack.h"
#include "App42Object.h"
#include "ACL.h"

 
class App42Service
{
public:
    
    App42Service();
    ~App42Service();
    
    void Init(std::string _apikey, std::string _secretkey);
    
    int pageOffset;
    int pageMaxRecords;
    bool metaInfo;

    std::string getSessionId(){return sessionId;}
    std::string getBaseUrl(std::string resource);
    std::string getAnalyticsUrl(string resource);
    std::string getCustomCodeUrl(std::string resource);
    std::string buildQueryString(std::map<std::string, std::string> queryParamsMap);
    std::string getFbAccessToken();
    std::string getAdminKey();
    
    std::string getOrderByDescending(){return orderByDescending;};
    void setOrderByDescending(std::string _orderByDescending){orderByDescending=_orderByDescending;};
    
    std::string getOrderByAscending(){return orderByAscending;};
    void setOrderByAscending(std::string _orderByAscending){orderByAscending=_orderByAscending;};
    void setFbAccessToken(std::string accessToken);
    void setAdminKey(std::string adminKey);
    void setSessionId(std::string _sessionId);

    void populateSignParams(std::map<std::string, std::string>& signParamsMap);
    void populateMetaHeaderParams(std::map<std::string, std::string>& signParamsMap);
    void setOtherMetaHeaders(std::map<std::string, std::string> l_otherMetaHearders);

    void setQuery(std::string _collectionName,Query *_metaInfoQuery);
    
    /**These methods are deprecated**/
    void setQuery(std::string _dbName, std::string _collectionName,Query *_metaInfoQuery);
    void setQuery(std::string _dbName, std::string _collectionName);
    /*****/
    
protected:
    std::string apiKey;
    std::string secretKey;
    std::string appVersion;
    std::string sessionId;
    std::string adminKey;
    std::string geoTag;
    std::string fbAccessToken;
    std::string orderByDescending;
    std::string orderByAscending;
    std::string event;
    std::string metaInfoQuery;
    std::string dbName;
    std::string collectionName;
    std::string query;
    std::string jsonObject;
    std::vector<std::string> selectKeys;
    std::vector<App42ACL> App42ACLList;
    std::map<std::string, std::string> otherMetaHearders;
    const char* getJsonStringFromVector(const char* key,vector<string>userList);
    const char* getJsonStringFromVector(vector<string>array);
    string getJsonStringFromApp42ACLList(string key,vector<App42ACL> &App42ACLObjectArray);

private:
    void setApp42ACLHeader(std::map<string, string>& metaHeaderParamsMap);
    string getJsonStringFromApp42ACLList(vector<App42ACL>& App42ACLObjectArray);

};

#endif /* defined(__App42CPPSDK__App42Service__) */
