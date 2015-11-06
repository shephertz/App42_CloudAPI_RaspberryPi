//
//  App42Object.cpp
//  App42Cocos2dX3.0Sample
//
//  Created by Rajeev Ranjan on 14/05/14.
//
//

#include "App42Object.h"

App42Object::App42Object()
{
    jsonObject = cJSON_CreateObject();
}

App42Object::~App42Object()
{
    cJSON_Delete(jsonObject);
}


void App42Object::release()
{
    
}

void App42Object::setObject(const char *key, bool value)
{
    if (value)
    {
        cJSON_AddTrueToObject(jsonObject, key);
    }
    else
    {
        cJSON_AddFalseToObject(jsonObject, key);
    }
}

void App42Object::setObject(const char *key, int value)
{
    cJSON_AddNumberToObject(jsonObject, key, value);
}

void App42Object::setObject(const char *key, float value)
{
    cJSON_AddNumberToObject(jsonObject, key, value);
}

void App42Object::setObject(const char *key, double value)
{
    cJSON_AddNumberToObject(jsonObject, key, value);
}

void App42Object::setObject(const char *key, const char *value)
{
    cJSON_AddStringToObject(jsonObject, key, value);
}

cJSON* App42Object::getJson()
{
    return jsonObject;
}

string App42Object::toString()
{
    char *cptrFormatted = cJSON_PrintUnformatted(jsonObject);
    string jsonString = cptrFormatted;
    free(cptrFormatted);
    return jsonString;
}