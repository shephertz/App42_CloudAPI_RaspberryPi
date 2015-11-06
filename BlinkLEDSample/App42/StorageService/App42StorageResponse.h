//
//  App42StorageResponse.h
//  App42CPPSDK
//
//  Created by Rajeev Ranjan on 21/10/13.
//
//

#ifndef __App42CPPSDK__App42StorageResponse__
#define __App42CPPSDK__App42StorageResponse__

#include <iostream>
#include "cJSON.h"
#include "App42Response.h"
#include "JSONDocument.h"

using namespace std;

struct App42Storage;

class App42StorageResponse : public App42Response
{
public:
    vector<App42Storage> storages;
    App42StorageResponse(const app42CallBack& pSelector);
	~App42StorageResponse();
    void onComplete(void *sender, void *data);
private:
    void init();
    
};


typedef struct App42Storage
{
    string dbName;
    string collectionName;
    double recordCount;
    vector<JSONDocument> jsonDocArray;
    
}App42Storage;

#endif /* defined(__App42CPPSDK__App42StorageResponse__) */
