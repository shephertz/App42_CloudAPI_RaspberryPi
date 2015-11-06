//
//  StorageService.cpp
//  App42CPPSDK
//
//  Created by Rajeev Ranjan on 21/10/13.
//
//

#include "StorageService.h"
#include "App42Constants.h"
#include "cJSON.h"
#include <map>
#include "Signing.h"
#include "BodyBuilder.h"
#include "App42StorageResponse.h"
#include "Connector.h"

using namespace App42Network;

// define the static..
StorageService* StorageService::_instance = NULL;

StorageService* StorageService::Initialize(string apikey, string secretkey)
{
	if(_instance == NULL)
    {
		_instance = new StorageService();
	}
    _instance->Init(apikey, secretkey);
    return _instance;
}

StorageService* StorageService::getInstance()
{
	return _instance;
}

void StorageService::Terminate()
{
	if(_instance != NULL)
    {
		delete _instance;
		_instance = NULL;
	}
}

StorageService::StorageService()
{
    
}

string BuildGrantAccessBody(string json)
{
	cJSON *bodyJSON = cJSON_CreateObject();
	cJSON *app42JSON = cJSON_CreateObject();
	cJSON *storageJSON = cJSON_CreateObject();
    
	cJSON_AddStringToObject(storageJSON, "App42ACLs", json.c_str());
    
	cJSON_AddItemReferenceToObject(app42JSON, "storage", storageJSON);
	cJSON_AddItemReferenceToObject(bodyJSON, "app42", app42JSON);
	char *cptrFormatted = cJSON_PrintUnformatted(bodyJSON);
	string bodyString = cptrFormatted;
    
	cJSON_Delete(storageJSON);
	cJSON_Delete(app42JSON);
	cJSON_Delete(bodyJSON);
	free(cptrFormatted);
	return bodyString;
}

string BuildStorageBody(string json)
{
	cJSON *bodyJSON = cJSON_CreateObject();
	cJSON *app42JSON = cJSON_CreateObject();
	cJSON *storageJSON = cJSON_CreateObject();
    
	cJSON_AddStringToObject(storageJSON, "jsonDoc", json.c_str());
	cJSON_AddItemReferenceToObject(app42JSON, "storage", storageJSON);
    
	cJSON_AddItemReferenceToObject(bodyJSON, "app42", app42JSON);
	char *cptrFormatted = cJSON_PrintUnformatted(bodyJSON);
	string bodyString = cptrFormatted;
    
	cJSON_Delete(storageJSON);
	cJSON_Delete(app42JSON);
	cJSON_Delete(bodyJSON);
	free(cptrFormatted);
	return bodyString;
}

string BuildStorageBody(string json, string queryJson)
{
	cJSON *bodyJSON = cJSON_CreateObject();
	cJSON *app42JSON = cJSON_CreateObject();
	cJSON *storageJSON = cJSON_CreateObject();
    
	cJSON_AddStringToObject(storageJSON, "jsonDoc", json.c_str());
    cJSON_AddStringToObject(storageJSON, "jsonQuery", queryJson.c_str());
	cJSON_AddItemReferenceToObject(app42JSON, "storage", storageJSON);
    
	cJSON_AddItemReferenceToObject(bodyJSON, "app42", app42JSON);
	char *cptrFormatted = cJSON_PrintUnformatted(bodyJSON);
	string bodyString = cptrFormatted;
    
	cJSON_Delete(storageJSON);
	cJSON_Delete(app42JSON);
	cJSON_Delete(bodyJSON);
	free(cptrFormatted);
	return bodyString;
}

string BuildStorageBody(cJSON *json, string queryJson)
{
	cJSON *bodyJSON = cJSON_CreateObject();
	cJSON *app42JSON = cJSON_CreateObject();
	cJSON *storageJSON = cJSON_CreateObject();
    
    cJSON_AddItemReferenceToObject(storageJSON, "jsonDoc", json);
    //cJSON_AddItemReferenceToObject(storageJSON, "jsonQuery", queryJson);
	//cJSON_AddStringToObject(storageJSON, "jsonDoc", json.c_str());
    cJSON_AddStringToObject(storageJSON, "jsonQuery", queryJson.c_str());
	cJSON_AddItemReferenceToObject(app42JSON, "storage", storageJSON);
    
	cJSON_AddItemReferenceToObject(bodyJSON, "app42", app42JSON);
	char *cptrFormatted = cJSON_PrintUnformatted(bodyJSON);
	string bodyString = cptrFormatted;
    
	cJSON_Delete(storageJSON);
	cJSON_Delete(app42JSON);
	cJSON_Delete(bodyJSON);
	free(cptrFormatted);
	return bodyString;
}


void StorageService::InsertJsonDocument(const char* dbName, const char* collectionName, const char* json,  const app42CallBack& pSelector)
{
    App42StorageResponse *response = new App42StorageResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(dbName, "Database Name");
        Util::throwExceptionIfStringNullOrBlank(collectionName, "Collection Name");
        Util::throwExceptionIfStringNullOrBlank(json, "Json String");
        ////Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    
    try {
        /**
         * Creating SignParams and signature
         */
        map<string, string> signParams;
        string timestamp = Util::getTimeStamp();
        populateSignParams(signParams);
        signParams["dbName"] = dbName;
        signParams["collectionName"] = collectionName;
        
        string storageBody = BuildStorageBody(json);
        signParams["body"] = storageBody;
        
        string signature = Util::signMap(secretKey, signParams);
        printf("\nSignParams : ");
        Util::printMap(signParams);
        /**
         * Creating URL
         */
        string resource = "storage/insert/dbName/";
        resource.append(dbName);
        resource.append("/collectionName/");
        resource.append(collectionName);
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
        Util::executePost(encodedUrl, headers, storageBody.c_str(),std::bind(&App42StorageResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));

    }
    catch (exception *e)
    {
        throw e;
    }
}

void StorageService::InsertJsonDocument(const char* dbName, const char* collectionName, App42Object *app42Object, const app42CallBack& pSelector)
{
    App42StorageResponse *response = new App42StorageResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(dbName, "Database Name");
        Util::throwExceptionIfStringNullOrBlank(collectionName, "Collection Name");
        Util::throwExceptionIfObjectIsNull(app42Object, "App42Object");
        ////Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    
    try {
        /**
         * Creating SignParams and signature
         */
        map<string, string> signParams;
        string timestamp = Util::getTimeStamp();
        populateSignParams(signParams);
        signParams["dbName"] = dbName;
        signParams["collectionName"] = collectionName;
        
        string storageBody = BuildStorageBody(app42Object->toString());
        signParams["body"] = storageBody;
        
        string signature = Util::signMap(secretKey, signParams);
        
        /**
         * Creating URL
         */
        string resource = "storage/insert/dbName/";
        resource.append(dbName);
        resource.append("/collectionName/");
        resource.append(collectionName);
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
        Util::executePost(encodedUrl, headers, storageBody.c_str(), std::bind(&App42StorageResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
        
    }
    catch (exception *e)
    {
        throw e;
    }
}


void StorageService::FindAllCollections(const char* dbName, const app42CallBack& pSelector)
{
    App42StorageResponse *response = new App42StorageResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(dbName, "Database Name");
        ////Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    map<string, string> signMap;
	populateSignParams( signMap);
    signMap["dbName"] = dbName;
	string signature = Util::signMap(secretKey, signMap);
    
    /**
     * Creating URL
     */
    string resource = "storage/findCollections/dbName/";
    resource.append(dbName);
	string url = getBaseUrl(resource);
    url.append("?");
    string encodedUrl = url_encode(url);
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
    Util::executeGet(encodedUrl,headers, std::bind(&App42StorageResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
}

void StorageService::FindAllDocuments(const char* dbName, const char* collectionName, const app42CallBack& pSelector)
{
    App42StorageResponse *response = new App42StorageResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(dbName, "Database Name");
        Util::throwExceptionIfStringNullOrBlank(collectionName, "Collection Name");
        ////Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    map<string, string> signMap;
	populateSignParams( signMap);
    signMap["dbName"] = dbName;
    signMap["collectionName"] = collectionName;
	string signature = Util::signMap(secretKey, signMap);
    
    /**
     * Creating URL
     */
    string resource = "storage/findAll/dbName/";
	resource.append(dbName);
    resource.append("/collectionName/");
	resource.append(collectionName);
	string url = getBaseUrl(resource);
    url.append("?");
    string encodedUrl = url_encode(url);
    /**
     * Creating Headers
     */
    //Util::app42Trace("\n baseUrl = %s",url.c_str());
    std::vector<std::string> headers;
    map<string, string> metaHeaders;
    populateMetaHeaderParams(metaHeaders);
    Util::BuildHeaders(metaHeaders, headers);
    
    Util::BuildHeaders(apiKey, timestamp, VERSION, signature, headers);
    /**
     * Initiating Http call
     */
    Util::executeGet(encodedUrl,headers, std::bind(&App42StorageResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));

}

void StorageService::FindAllDocuments(const char* dbName, const char* collectionName,int max, int offset, const app42CallBack& pSelector)
{
    App42StorageResponse *response = new App42StorageResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(dbName, "Database Name");
        Util::throwExceptionIfStringNullOrBlank(collectionName, "Collection Name");
        ////Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    map<string, string> signMap;
	populateSignParams( signMap);
    signMap["dbName"] = dbName;
    signMap["collectionName"] = collectionName;
    signMap["max"] = Util::ItoA(max);
    signMap["offset"] = Util::ItoA(offset);
	string signature = Util::signMap(secretKey, signMap);
    
    /**
     * Creating URL
     */
    string resource = "storage/findAll/dbName/";
	resource.append(dbName);
    resource.append("/collectionName/");
	resource.append(collectionName);
    resource.append("/"+Util::ItoA(max));
    resource.append("/"+Util::ItoA(offset));
	string url = getBaseUrl(resource);
    url.append("?");
    string encodedUrl = url_encode(url);
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
    Util::executeGet(encodedUrl,headers, std::bind(&App42StorageResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
    
}


void StorageService::FindAllDocumentsCount(const char* dbName, const char* collectionName, const app42CallBack& pSelector)
{
    App42StorageResponse *response = new App42StorageResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(dbName, "Database Name");
        Util::throwExceptionIfStringNullOrBlank(collectionName, "Collection Name");
        ////Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    map<string, string> signMap;
	populateSignParams( signMap);
    signMap["dbName"] = dbName;
    signMap["collectionName"] = collectionName;
	string signature = Util::signMap(secretKey, signMap);
    /**
     * Creating URL
     */
    string resource = "storage/findAll/count/dbName/";
	resource.append(dbName);
    resource.append("/collectionName/");
	resource.append(collectionName);
	string url = getBaseUrl(resource);
    url.append("?");
    string encodedUrl = url_encode(url);
    
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
    Util::executeGet(encodedUrl,headers, std::bind(&App42StorageResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
    
}

void StorageService::FindDocumentById(const char* dbName, const char* collectionName, const char* docId, const app42CallBack& pSelector)
{
    App42StorageResponse *response = new App42StorageResponse(pSelector);
    printf("Docid = %s",docId);
    try
    {
        Util::throwExceptionIfStringNullOrBlank(dbName, "Database Name");
        Util::throwExceptionIfStringNullOrBlank(collectionName, "Collection Name");
        Util::throwExceptionIfStringNullOrBlank(docId, "Doc ID");
        ////Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    map<string, string> signMap;
	populateSignParams( signMap);
    signMap["dbName"] = dbName;
    signMap["collectionName"] = collectionName;
    signMap["docId"] = docId;
	string signature = Util::signMap(secretKey, signMap);
    
    /**
     * Creating URL
     */
    string resource = "storage/findDocById/dbName/";
	resource.append(dbName);
    resource.append("/collectionName/");
	resource.append(collectionName);
    resource.append("/docId/");
	resource.append(docId);
	string url = getBaseUrl(resource);
    url.append("?");
    string encodedUrl = url_encode(url);
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
    Util::executeGet(encodedUrl,headers, std::bind(&App42StorageResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));

}

void StorageService::FindDocumentByQuery(const char* dbName, const char* collectionName, Query *query, const app42CallBack& pSelector)
{
    FindDocumentsByQuery(dbName, collectionName, query, pSelector);
}

void StorageService::FindDocumentsByQuery(const char* dbName, const char* collectionName, Query *query, const app42CallBack& pSelector)
{
    App42StorageResponse *response = new App42StorageResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(dbName, "Database Name");
        Util::throwExceptionIfStringNullOrBlank(collectionName, "Collection Name");
        ////Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    map<string, string> signMap;
	populateSignParams( signMap);
    signMap["dbName"] = dbName;
    signMap["collectionName"] = collectionName;
    signMap["jsonQuery"] = query->getString();
	string signature = Util::signMap(secretKey, signMap);
    printf("\nSignParams:\n");
    Util::printMap(signMap);
    printf("\n");

    /**
     * Creating URL
     */
    string resource = "storage/findDocsByQuery/dbName/";
	resource.append(dbName);
    resource.append("/collectionName/");
	resource.append(collectionName);
	string url = getBaseUrl(resource);
    url.append("?");
    /**
     * Creating QueryParams
     */
    map<string, string> queryParamsMap;
    queryParamsMap["jsonQuery"]=query->getString();
    string queryString = buildQueryString(queryParamsMap);
    url.append(queryString);
    string encodedUrl = url_encode(url);

    
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
    Util::executeGet(encodedUrl,headers, std::bind(&App42StorageResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
    
}

void StorageService::FindDocumentsByQueryWithPaging(const char* dbName, const char* collectionName, Query *query, int max, int offset, const app42CallBack& pSelector)
{
    App42StorageResponse *response = new App42StorageResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(dbName, "Database Name");
        Util::throwExceptionIfStringNullOrBlank(collectionName, "Collection Name");
        ////Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
        Util::throwExceptionIfCallBackIsNull(pSelector, "Callback");
        Util::throwExceptionIfMaxIsNotValid(max, "Max");
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
    map<string, string> signMap;
	populateSignParams( signMap);
    signMap["dbName"] = dbName;
    signMap["collectionName"] = collectionName;
    signMap["jsonQuery"] = query->getString();
    signMap["max"] = Util::ItoA(max);
    signMap["offset"] = Util::ItoA(offset);
	string signature = Util::signMap(secretKey, signMap);
    
    /**
     * Creating URL
     */
    string resource = "storage/findDocsByQuery/dbName/";
	resource.append(dbName );
    resource.append("/collectionName/");
	resource.append(collectionName);
    resource.append("/"+Util::ItoA(max));
    resource.append("/"+Util::ItoA(offset));
	string url = getBaseUrl(resource);
    url.append("?");
    /**
     * Creating QueryParams
     */
    map<string, string> queryParamsMap;
    queryParamsMap["jsonQuery"]=query->getString();
    string queryString = buildQueryString(queryParamsMap);
    url.append(queryString);
    string encodedUrl = url_encode(url);

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
    Util::executeGet(encodedUrl,headers, std::bind(&App42StorageResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
    
}


void StorageService::FindDocsWithQueryPagingOrderBy(const char* dbName, const char* collectionName, Query *query, int max, int offset,const char* orderByKey,const char* orderByType, const app42CallBack& pSelector)
{
    App42StorageResponse *response = new App42StorageResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(dbName, "Database Name");
        Util::throwExceptionIfStringNullOrBlank(collectionName, "Collection Name");
        ////Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    map<string, string> signMap;
	populateSignParams( signMap);
    signMap["dbName"] = dbName;
    signMap["collectionName"] = collectionName;
    signMap["jsonQuery"] = query->getString();
    signMap["max"] = Util::ItoA(max);
    signMap["offset"] = Util::ItoA(offset);
	string signature = Util::signMap(secretKey, signMap);
    
    /**
     * Creating URL
     */
    string resource = "storage/findDocsByQuery/dbName/";
	resource.append(dbName);
    resource.append("/collectionName/");
	resource.append(collectionName);
    resource.append("/"+Util::ItoA(max));
    resource.append("/"+Util::ItoA(offset));
	string url = getBaseUrl(resource);
    url.append("?");
    
    /**
     * Creating QueryParams
     */
    map<string, string> queryParamsMap;
    if (orderByKey!= NULL)
    {
        queryParamsMap["orderByKey"]=orderByKey;
    }
    if (orderByType!= NULL)
    {
        queryParamsMap["orderByType"]=orderByType;
    }
    queryParamsMap["jsonQuery"]=query->getString();
    string queryString = buildQueryString(queryParamsMap);
    url.append(queryString);
    string encodedUrl = url_encode(url);

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
    Util::executeGet(encodedUrl,headers, std::bind(&App42StorageResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
    
}

void StorageService::FindDocumentByKeyValue(const char* dbName, const char* collectionName, const char* key,const char* value, const app42CallBack& pSelector)
{
    App42StorageResponse *response = new App42StorageResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(dbName, "Database Name");
        Util::throwExceptionIfStringNullOrBlank(collectionName, "Collection Name");
        Util::throwExceptionIfStringNullOrBlank(key, "Key");
        Util::throwExceptionIfStringNullOrBlank(value, "Value");
        ////Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    map<string, string> signMap;
	populateSignParams( signMap);
    signMap["dbName"] = dbName;
    signMap["collectionName"] = collectionName;
    signMap["key"] = key;
	signMap["value"] = value;
	string signature = Util::signMap(secretKey, signMap);
    
    /**
     * Creating URL
     */
    string resource = "storage/findDocByKV/dbName/";
	resource.append(dbName);
    resource.append("/collectionName/");
	resource.append(collectionName);
    resource.append("/");
	resource.append(key);
    resource.append("/");
	resource.append(value);
	string url = getBaseUrl(resource);
    url.append("?");
    string encodedUrl = url_encode(url);
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
    Util::executeGet(encodedUrl,headers, std::bind(&App42StorageResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));

}


void StorageService::UpdateDocumentByDocId(const char* dbName, const char* collectionName, const char* docId,const char* json, const app42CallBack& pSelector)
{
    App42StorageResponse *response = new App42StorageResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(dbName, "Database Name");
        Util::throwExceptionIfStringNullOrBlank(collectionName, "Collection Name");
        Util::throwExceptionIfStringNullOrBlank(docId, "Doc ID");
        Util::throwExceptionIfStringNullOrBlank(json, "Json String");
        ////Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    map<string, string> postMap;
    string timestamp = Util::getTimeStamp();
    populateSignParams(postMap);
    postMap["dbName"] = dbName;
	postMap["collectionName"] = collectionName;
    postMap["docId"] = docId;
    string storageBody = BuildStorageBody(json);
    postMap["body"] = storageBody;
    
    string signature = Util::signMap(secretKey, postMap);
    
    /**
     * Creating URL
     */
    string resource = "storage/updateByDocId/dbName/";
    resource.append(dbName);
    resource.append("/collectionName/");
	resource.append(collectionName);
    resource.append("/docId/");
    resource.append(docId);
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
    Util::executePut(encodedUrl, headers, storageBody.c_str(), std::bind(&App42StorageResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
}


void StorageService::UpdateDocumentByDocId(const char* dbName, const char* collectionName, const char* docId,App42Object *app42Object, const app42CallBack& pSelector)
{
    App42StorageResponse *response = new App42StorageResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(dbName, "Database Name");
        Util::throwExceptionIfStringNullOrBlank(collectionName, "Collection Name");
        Util::throwExceptionIfStringNullOrBlank(docId, "Doc ID");
        Util::throwExceptionIfObjectIsNull(app42Object, "App42Object");
        ////Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    map<string, string> postMap;
    string timestamp = Util::getTimeStamp();
    populateSignParams(postMap);
    postMap["dbName"] = dbName;
	postMap["collectionName"] = collectionName;
    postMap["docId"] = docId;
    string storageBody = BuildStorageBody(app42Object->toString());
    postMap["body"] = storageBody;
    
    string signature = Util::signMap(secretKey, postMap);
    
    /**
     * Creating URL
     */
    string resource = "storage/updateByDocId/dbName/";
    resource.append(dbName );
    resource.append("/collectionName/");
	resource.append(collectionName);
    resource.append("/docId/");
    resource.append(docId);
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
    Util::executePut(encodedUrl, headers, storageBody.c_str(), std::bind(&App42StorageResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
}

void StorageService::UpdateDocumentByKeyValue(const char* dbName, const char* collectionName, const char* key,const char* value,const char* json, const app42CallBack& pSelector)
{
    App42StorageResponse *response = new App42StorageResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(dbName, "Database Name");
        Util::throwExceptionIfStringNullOrBlank(collectionName, "Collection Name");
        Util::throwExceptionIfStringNullOrBlank(key, "Key");
        Util::throwExceptionIfStringNullOrBlank(value, "Value");
        Util::throwExceptionIfStringNullOrBlank(json, "Json String");
        ////Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    signParams["dbName"] = dbName;
	signParams["collectionName"] = collectionName;
    signParams["key"] = key;
    signParams["value"] = value;
    string storageBody = BuildStorageBody(json);
    signParams["body"] = storageBody;
    
    string signature = Util::signMap(secretKey, signParams);
    
    /**
     * Creating URL
     */
    string resource = "storage/update/dbName/";
    resource.append(dbName);
    resource.append("/collectionName/");
	resource.append(collectionName);
    resource.append("/");
    resource.append(key);
    resource.append("/");
    resource.append(value);
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
    Util::executePut(encodedUrl, headers, storageBody.c_str(), std::bind(&App42StorageResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
}

void StorageService::UpdateDocumentByKeyValue(const char* dbName, const char* collectionName, const char* key,const char* value,App42Object *app42Object, const app42CallBack& pSelector)
{
    App42StorageResponse *response = new App42StorageResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(dbName, "Database Name");
        Util::throwExceptionIfStringNullOrBlank(collectionName, "Collection Name");
        Util::throwExceptionIfStringNullOrBlank(key, "Key");
        Util::throwExceptionIfStringNullOrBlank(value, "Value");
        Util::throwExceptionIfObjectIsNull(app42Object, "App42Object");
        ////Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    map<string, string> postMap;
    string timestamp = Util::getTimeStamp();
    populateSignParams(postMap);
    postMap["dbName"] = dbName;
	postMap["collectionName"] = collectionName;
    postMap["key"] = key;
    postMap["value"] = value;
    string storageBody = BuildStorageBody(app42Object->toString());
    postMap["body"] = storageBody;
    
    string signature = Util::signMap(secretKey, postMap);
    
    /**
     * Creating URL
     */
    string resource = "storage/update/dbName/";
    resource.append(dbName);
    resource.append("/collectionName/");
	resource.append(collectionName);
    resource.append("/");
    resource.append(key);
    resource.append("/");
    resource.append(value);
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
    Util::executePut(encodedUrl, headers, storageBody.c_str(), std::bind(&App42StorageResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
}


void StorageService::SaveOrUpdateDocumentByKeyValue(const char* dbName, const char* collectionName, const char* key,const char* value,const char* json, const app42CallBack& pSelector)
{
    App42StorageResponse *response = new App42StorageResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(dbName, "Database Name");
        Util::throwExceptionIfStringNullOrBlank(collectionName, "Collection Name");
        Util::throwExceptionIfStringNullOrBlank(key, "Key");
        Util::throwExceptionIfStringNullOrBlank(value, "Value");
        Util::throwExceptionIfStringNullOrBlank(json, "Json String");
        ////Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    map<string, string> postMap;
    string timestamp = Util::getTimeStamp();
    populateSignParams(postMap);
    postMap["dbName"] = dbName;
	postMap["collectionName"] = collectionName;
    postMap["key"] = key;
    postMap["value"] = value;
    string storageBody = BuildStorageBody(json);
    postMap["body"] = storageBody;
    string signature = Util::signMap(secretKey, postMap);
    
    /**
     * Creating URL
     */
    string resource = "storage/saveorupdate/dbName/";
    resource.append(dbName);
    resource.append("/collectionName/");
	resource.append(collectionName);
    resource.append("/");
    resource.append(key);
    resource.append("/");
    resource.append(value);
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
    Util::executePut(encodedUrl, headers, storageBody.c_str(), std::bind(&App42StorageResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
}

void StorageService::SaveOrUpdateDocumentByKeyValue(const char* dbName, const char* collectionName, const char* key,const char* value,App42Object *app42Object, const app42CallBack& pSelector)
{
    App42StorageResponse *response = new App42StorageResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(dbName, "Database Name");
        Util::throwExceptionIfStringNullOrBlank(collectionName, "Collection Name");
        Util::throwExceptionIfStringNullOrBlank(key, "Key");
        Util::throwExceptionIfStringNullOrBlank(value, "Value");
        Util::throwExceptionIfObjectIsNull(app42Object, "App42Object");
        ////Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    map<string, string> postMap;
    string timestamp = Util::getTimeStamp();
    populateSignParams(postMap);
    postMap["dbName"] = dbName;
	postMap["collectionName"] = collectionName;
    postMap["key"] = key;
    postMap["value"] = value;
    string storageBody = BuildStorageBody(app42Object->toString());
    postMap["body"] = storageBody;
    string signature = Util::signMap(secretKey, postMap);
    
    /**
     * Creating URL
     */
    string resource = "storage/saveorupdate/dbName/";
    resource.append(dbName);
    resource.append("/collectionName/");
	resource.append(collectionName);
    resource.append("/");
    resource.append(key);
    resource.append("/");
    resource.append(value);
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
    Util::executePut(encodedUrl, headers, storageBody.c_str(), std::bind(&App42StorageResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
}



void StorageService::DeleteDocumentsById(const char* dbName, const char* collectionName, const char* docId, const app42CallBack& pSelector)
{
    App42StorageResponse *response = new App42StorageResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(dbName, "Database Name");
        Util::throwExceptionIfStringNullOrBlank(collectionName, "Collection Name");
        Util::throwExceptionIfStringNullOrBlank(docId, "Doc ID");
        ////Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    map<string, string> signMap;
	populateSignParams( signMap);
    signMap["dbName"] = dbName;
    signMap["collectionName"] = collectionName;
    signMap["docId"] = docId;
	string signature = Util::signMap(secretKey, signMap);
    
    /**
     * Creating URL
     */
    string resource = "storage/deleteDocById/dbName/";
	resource.append(dbName);
    resource.append("/collectionName/");
	resource.append(collectionName);
    resource.append("/docId/");
	resource.append(docId);
    
	string url = getBaseUrl(resource);
    url.append("?");
    string encodedUrl = url_encode(url);
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
    Util::executeDelete(encodedUrl,headers, std::bind(&App42StorageResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
    
}

void StorageService::DeleteDocumentsByKeyValue(const char* dbName, const char* collectionName, const char* key, const char* value, const app42CallBack& pSelector)
{
    App42StorageResponse *response = new App42StorageResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(dbName, "Database Name");
        Util::throwExceptionIfStringNullOrBlank(collectionName, "Collection Name");
        Util::throwExceptionIfStringNullOrBlank(key, "Key");
        Util::throwExceptionIfStringNullOrBlank(value, "Value");
        ////Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    map<string, string> queryParamsMap;
    
    /**
     * Creating SignParams and signature
     */
    map<string, string> signMap;
	populateSignParams( signMap);
    
    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "key", value);
    char *cptrFormatted = cJSON_PrintUnformatted(json);
	string bodyString = cptrFormatted;
    free(cptrFormatted);
    cJSON_Delete(json);
    signMap["value"] = bodyString;
    queryParamsMap["value"] = bodyString;
    
    signMap["dbName"] = dbName;
    signMap["collectionName"] = collectionName;
    signMap["key"] = key;
	string signature = Util::signMap(secretKey, signMap);
    
    /**
     * Creating URL
     */
    string resource = "storage/deletebykey/dbName/";
	resource.append(dbName);
    resource.append("/collectionName/");
	resource.append(collectionName);
    resource.append("/");
	resource.append(key);
	string url = getBaseUrl(resource);
    url.append("?");
    /**
     * Creating QueryParams
     */
    string queryString = buildQueryString(queryParamsMap);
    url.append(queryString);
    string encodedUrl = url_encode(url);

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
    Util::executeDelete(encodedUrl,headers, std::bind(&App42StorageResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
    
}

void StorageService::DeleteAllDocuments(const char* dbName, const char* collectionName, const app42CallBack& pSelector)
{
    App42StorageResponse *response = new App42StorageResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(dbName, "Database Name");
        Util::throwExceptionIfStringNullOrBlank(collectionName, "Collection Name");
        ////Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    map<string, string> signMap;
	populateSignParams( signMap);
    signMap["dbName"] = dbName;
    signMap["collectionName"] = collectionName;
	string signature = Util::signMap(secretKey, signMap);
    
    /**
     * Creating URL
     */
    string resource = "storage/deleteAll/dbName/";
	resource.append(dbName);
    resource.append("/collectionName/");

	resource.append(collectionName);
	string url = getBaseUrl(resource);
    url.append("?");
    string encodedUrl = url_encode(url);
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
    Util::executeDelete(encodedUrl,headers, std::bind(&App42StorageResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
    
}

void StorageService::AddOrUpdateKeys(const char* dbName, const char* collectionName, const char* docId,const char* json, const app42CallBack& pSelector)
{
    App42StorageResponse *response = new App42StorageResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(dbName, "Database Name");
        Util::throwExceptionIfStringNullOrBlank(collectionName, "Collection Name");
        Util::throwExceptionIfStringNullOrBlank(json, "Json String");
        ////Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    
    try {
        /**
         * Creating SignParams and signature
         */
        map<string, string> signParams;
        string timestamp = Util::getTimeStamp();
        populateSignParams(signParams);
        signParams["dbName"] = dbName;
        signParams["collectionName"] = collectionName;
        signParams["docId"] = docId;
        string storageBody = BuildStorageBody(json);
        signParams["body"] = storageBody;
        
        string signature = Util::signMap(secretKey, signParams);
        
        /**
         * Creating URL
         */
        string resource = "storage/updateKeysByDocId/dbName/";
        resource.append(dbName);
        resource.append("/collectionName/");
        resource.append(collectionName);
        resource.append("/docId/");
        resource.append(docId);
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
        Util::executePut(encodedUrl, headers, storageBody.c_str(), std::bind(&App42StorageResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
        
    }
    catch (exception *e)
    {
        throw e;
    }
}

void StorageService::AddOrUpdateKeys(const char* dbName, const char* collectionName, const char* docId,App42Object *app42Object, const app42CallBack& pSelector)
{
    App42StorageResponse *response = new App42StorageResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(dbName, "Database Name");
        Util::throwExceptionIfStringNullOrBlank(collectionName, "Collection Name");
        Util::throwExceptionIfObjectIsNull(app42Object, "App42Object");
        ////Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    
    try {
        /**
         * Creating SignParams and signature
         */
        map<string, string> signParams;
        string timestamp = Util::getTimeStamp();
        populateSignParams(signParams);
        signParams["dbName"] = dbName;
        signParams["collectionName"] = collectionName;
        signParams["docId"] = docId;
        string storageBody = BuildStorageBody(app42Object->toString());
        signParams["body"] = storageBody;
        
        string signature = Util::signMap(secretKey, signParams);
        
        /**
         * Creating URL
         */
        string resource = "storage/updateKeysByDocId/dbName/";
        resource.append(dbName);
        resource.append("/collectionName/");
        resource.append(collectionName);
        resource.append("/docId/");
        resource.append(docId);
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
        Util::executePut(encodedUrl, headers, storageBody.c_str(), std::bind(&App42StorageResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
        
    }
    catch (exception *e)
    {
        throw e;
    }
}

void StorageService::UpdateDocumentByQuery(const char* dbName, const char* collectionName, Query *query,const char* json, const app42CallBack& pSelector)
{
    App42StorageResponse *response = new App42StorageResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(dbName, "Database Name");
        Util::throwExceptionIfStringNullOrBlank(collectionName, "Collection Name");
        Util::throwExceptionIfStringNullOrBlank(json, "Json String");
        ////Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    
    try {
        /**
         * Creating SignParams and signature
         */
        map<string, string> signParams;
        string timestamp = Util::getTimeStamp();
        populateSignParams(signParams);
        signParams["dbName"] = dbName;
        signParams["collectionName"] = collectionName;
        string storageBody = BuildStorageBody(json,query->getString());
        signParams["body"] = storageBody;
        
        Util::printMap(signParams);
        string signature = Util::signMap(secretKey, signParams);
        
        /**
         * Creating URL
         */
        string resource = "storage/updateDocsByQuery/dbName/";
        resource.append(dbName);
        resource.append("/collectionName/");
        resource.append(collectionName);
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
        Util::executePut(encodedUrl, headers, storageBody.c_str(), std::bind(&App42StorageResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
        
    }
    catch (exception *e)
    {
        throw e;
    }
}

void StorageService::UpdateDocumentByQuery(const char* dbName, const char* collectionName, Query *query, App42Object *app42Object, const app42CallBack& pSelector)
{
    App42StorageResponse *response = new App42StorageResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(dbName, "Database Name");
        Util::throwExceptionIfStringNullOrBlank(collectionName, "Collection Name");
        Util::throwExceptionIfObjectIsNull(app42Object, "App42Object");
        ////Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    
    try {
        /**
         * Creating SignParams and signature
         */
        map<string, string> signParams;
        string timestamp = Util::getTimeStamp();
        populateSignParams(signParams);
        signParams["dbName"] = dbName;
        signParams["collectionName"] = collectionName;
        string storageBody = BuildStorageBody(app42Object->getJson(),query->getString());
        signParams["body"] = storageBody;
        
        string signature = Util::signMap(secretKey, signParams);

        Util::printMap(signParams);
        /**
         * Creating URL
         */
        string resource = "storage/updateDocsByQuery/dbName/";
        resource.append(dbName);
        resource.append("/collectionName/");
        resource.append(collectionName);
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
        Util::executePut(encodedUrl, headers, storageBody.c_str(), std::bind(&App42StorageResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
        
    }
    catch (exception *e)
    {
        throw e;
    }
}

void StorageService::GrantAccessOnDoc(const char* dbName, const char* collectionName,
                      const char* docId, vector<App42ACL> App42ACLList, const app42CallBack& pSelector)
{
    App42StorageResponse *response = new App42StorageResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(dbName, "Database Name");
        Util::throwExceptionIfStringNullOrBlank(collectionName, "Collection Name");
        Util::throwExceptionIfStringNullOrBlank(docId, "DocId");
        //Util::throwExceptionIfVectorIsNullOrBlank(App42ACLList, "App42ACLList");

        ////Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    /*body={"app42":{"storage":{"App42ACLs":"{\"App42ACL\":[{\"Rajeev\":\"R\"}]}"}}}*/
    try {
        /**
         * Creating SignParams and signature
         */
        map<string, string> signParams;
        string timestamp = Util::getTimeStamp();
        populateSignParams(signParams);
        signParams["dbName"] = dbName;
        signParams["collectionName"] = collectionName;
        signParams["docId"] = docId;

        string App42ACLJsonString = getJsonStringFromApp42ACLList("App42ACL",App42ACLList);
        string storageBody = BuildGrantAccessBody(App42ACLJsonString);
        signParams["body"] = storageBody;
        
        string signature = Util::signMap(secretKey, signParams);
        
        Util::printMap(signParams);
        /**
         * Creating URL
         */
        string resource = "storage/grantAccessOnDoc/";
        resource.append(dbName);
        resource.append("/");
        resource.append(collectionName);
        resource.append("/");
        resource.append(docId);
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
        Util::executePut(encodedUrl, headers, storageBody.c_str(), std::bind(&App42StorageResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
        
    }
    catch (exception *e)
    {
        throw e;
    }
}

void StorageService::RevokeAccessOnDoc(const char* dbName, const char* collectionName,
                       const char* docId, vector<App42ACL> App42ACLList, const app42CallBack& pSelector)
{
    App42StorageResponse *response = new App42StorageResponse(pSelector);
    
    try
    {
        Util::throwExceptionIfStringNullOrBlank(dbName, "Database Name");
        Util::throwExceptionIfStringNullOrBlank(collectionName, "Collection Name");
        Util::throwExceptionIfStringNullOrBlank(docId, "DocId");
        //Util::throwExceptionIfVectorIsNullOrBlank(App42ACLList, "App42ACLList");
        
        ////Util::throwExceptionIfTargetIsNull(pTarget, "Callback's Target");
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
    
    try {
        /**
         * Creating SignParams and signature
         */
        map<string, string> signParams;
        string timestamp = Util::getTimeStamp();
        populateSignParams(signParams);
        signParams["dbName"] = dbName;
        signParams["collectionName"] = collectionName;
        signParams["docId"] = docId;
        
        string App42ACLJsonString = getJsonStringFromApp42ACLList("App42ACL",App42ACLList);
        string storageBody = BuildGrantAccessBody(App42ACLJsonString);
        signParams["body"] = storageBody;
        
        string signature = Util::signMap(secretKey, signParams);
        
        Util::printMap(signParams);
        /**
         * Creating URL
         */
        string resource = "storage/revokeAccessOnDoc/";
        resource.append(dbName);
        resource.append("/");
        resource.append(collectionName);
        resource.append("/");
        resource.append(docId);
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
        Util::executePut(encodedUrl, headers, storageBody.c_str(), std::bind(&App42StorageResponse::onComplete, response, std::placeholders::_1, std::placeholders::_2));
        
    }
    catch (exception *e)
    {
        throw e;
    }
}
