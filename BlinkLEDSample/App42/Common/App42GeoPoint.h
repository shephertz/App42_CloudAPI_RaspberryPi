//
//  App42GeoPoint.h
//  App42Cocos2dX3.0Sample
//
//  Created by Shephertz Technologies Pvt Ltd on 24/07/14.
//
//

#ifndef __App42Cocos2dX3_0Sample__App42GeoPoint__
#define __App42Cocos2dX3_0Sample__App42GeoPoint__

#include <iostream>
#include "cJSON.h"

using namespace std;

class App42GeoPoint
{
public:
    App42GeoPoint();
    ~App42GeoPoint();
        
    double latitude;
    double longitude;
    string buddyName;
    string markerName;
    string createdOn;
    
    string toString();
    cJSON* getJson(); // You need to free the returned cJSON object
private:
};

#endif /* defined(__App42Cocos2dX3_0Sample__App42GeoPoint__) */
