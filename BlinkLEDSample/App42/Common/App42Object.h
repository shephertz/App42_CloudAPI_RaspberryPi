//
//  App42Object.h
//  App42Cocos2dX3.0Sample
//
//  Created by Rajeev Ranjan on 14/05/14.
//
//

#ifndef __App42Cocos2dX3_0Sample__App42Object__
#define __App42Cocos2dX3_0Sample__App42Object__

#include <iostream>
#include "cJSON.h"

using namespace std;

class App42Object
{
public:
    App42Object();
    ~App42Object();
    
    void release();
    
    void setObject(const char *key, bool value);
    void setObject(const char *key, int value);
    void setObject(const char *key, float value);
    void setObject(const char *key, double value);
    void setObject(const char *key, long value);
    void setObject(const char *key, const char *value);
    
    string toString();
    cJSON* getJson();
private:
    cJSON *jsonObject;
};

#endif /* defined(__App42Cocos2dX3_0Sample__App42Object__) */
