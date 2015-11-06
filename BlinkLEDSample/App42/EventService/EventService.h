//
//  EventService.h
//  App42Cocos2dXSample
//
//  Created by Rajeev Ranjan on 02/07/15.
//
//

#ifndef __App42Cocos2dXSample__EventService__
#define __App42Cocos2dXSample__EventService__

#include <stdio.h>
#include <iostream>
#include "App42Service.h"

using namespace std;

class EventService: public App42Service
{
private:
    static EventService* _instance;
    EventService();
    string BuildTrackEventBody(string eventName, App42Object* properties);
    string BuildSetPropBody(App42Object *properties);
    string BuildUpdatePropBody(App42Object *properties);
    App42Object* GetSuperProperties();
    App42Object* GetInstallProperties();
    string getPlatformName();
    void updateApp42RequestCounter(int counter);
public:
    /**
     * This is a constructor that takes
     *
     * @param apiKey
     * @param secretKey
     * @param server
     *
     */
    
    static EventService* Initialize(string apikey, string secretkey);
    
    static void Terminate();
    
    /**
     * Builds the instance of UserService.
     *
     * @return UserService - UserService Object
     */
    static EventService* getInstance();
    
    /**
     * Adds game score for the specified user.
     *
     * @param gameName - Name of the game for which scores have to be added.
     * @param userName - The user for whom scores have to be added.
     * @param score - The scores that have to be added.
     * @return app42Result - The result of the request.
     */
    void TrackEvent(const char* eventName, App42Object *properties, const app42CallBack& pSelector);
    void SetLoggedInUserProperties(App42Object *properties, const app42CallBack& pSelector);
    void UpdateLoggedInUserProperties(App42Object *properties, const app42CallBack& pSelector);
};

#endif /* defined(__App42Cocos2dXSample__EventService__) */
