//
//  App42StorageResponse.cpp
//  App42CPPSDK
//
//  Created by Rajeev Ranjan on 21/10/13.
//
//

#include "App42StorageResponse.h"
#include "Common.h"


App42StorageResponse::App42StorageResponse(const app42CallBack& pSelector)
:App42Response(pSelector)
{
    
}

App42StorageResponse::~App42StorageResponse()
{
    storages.clear();
}

void App42StorageResponse::onComplete(void *sender, void *data)
{
    App42Response::onComplete(sender, data);
    init();
    
    
    if (_app42Selector)
    {
        _app42Selector(this);
    }
}


void App42StorageResponse::init()
{
    if(_result != 200)
    {
        Util::app42Trace("App42StorageResult failed result is %d", _result);
        buildErrorMessage();
        return;
    }
    // parse the body
    cJSON *ptrBody = cJSON_Parse(_body.c_str());
    cJSON* ptrApp42 = Util::getJSONChild("app42", ptrBody);
    cJSON* ptrResponse = Util::getJSONChild("response", ptrApp42);
    cJSON* ptrStorages = Util::getJSONChild("storage", ptrResponse);
    if (ptrStorages)
    {
        cJSON* child = ptrStorages;
        if(child->type == cJSON_Array)
        {
            child = child->child;
        }
        
        while(child != NULL && child->type == cJSON_Object)
        {
            App42Storage app42Storage;
            app42Storage.dbName = Util::getJSONString("dbName", child);
            app42Storage.collectionName = Util::getJSONString("collectionName", child);
            app42Storage.recordCount = Util::getJSONDouble("recordCount", child);
            
            child = child->next;
            
            if(Util::getJSONChild("jsonDoc",ptrStorages))
            {
                cJSON* ptrjsonDocs = Util::getJSONChild("jsonDoc", ptrStorages);
                cJSON* child = ptrjsonDocs;
                if(child->type == cJSON_Array)
                {
                    child = child->child;
                }
                while(child != NULL && child->type == cJSON_Object)
                {
                    JSONDocument document;
                    buildJsonDocument(child, &document);
                    
                    app42Storage.jsonDocArray.push_back(document);
                    child = child->next;
                }
            }
            storages.push_back(app42Storage);
        }
    }
    else
    {
        setTotalRecords();
    }
    
    cJSON_Delete(ptrBody);
}