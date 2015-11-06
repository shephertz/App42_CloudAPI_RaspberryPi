//
//  App42GeoPoint.cpp
//  App42Cocos2dX3.0Sample
//
//  Created by Shephertz Technologies Pvt Ltd on 24/07/14.
//
//

#include "App42GeoPoint.h"


App42GeoPoint::App42GeoPoint()
{
    
}

App42GeoPoint::~App42GeoPoint()
{
    
}


cJSON* App42GeoPoint::getJson()
{
    cJSON* jsonObject = cJSON_CreateObject();
    
    cJSON_AddNumberToObject(jsonObject, "lat", latitude);
    cJSON_AddNumberToObject(jsonObject, "lng", longitude);
    cJSON_AddStringToObject(jsonObject, "marker", markerName.c_str());
    
    return jsonObject;
}

string App42GeoPoint::toString()
{
    cJSON* jsonObject = cJSON_CreateObject();
    
    cJSON_AddNumberToObject(jsonObject, "lat", latitude);
    cJSON_AddNumberToObject(jsonObject, "lng", longitude);
    cJSON_AddStringToObject(jsonObject, "marker", markerName.c_str());

    char *cptrFormatted = cJSON_PrintUnformatted(jsonObject);
    string jsonString = cptrFormatted;
    free(cptrFormatted);
    
    cJSON_Delete(jsonObject);
    
    return jsonString;
}