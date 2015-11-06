//
//  App42PushNotificationResponse.h
//  App42CPPSDK
//
//  Created by Rajeev Ranjan on 21/10/13.
//
//

#ifndef __App42CPPSDK__App42PushNotificationResponse__
#define __App42CPPSDK__App42PushNotificationResponse__

#include <iostream>
#include "cJSON.h"
#include "App42Response.h"

using namespace std;

struct App42PushNotification;
struct App42Channel;

class App42PushNotificationResponse : public App42Response
{
public:
    vector<App42PushNotification> notifications;
    App42PushNotificationResponse(const app42CallBack& pSelector);
	~App42PushNotificationResponse();
    void onComplete(void *sender, void *data);
private:
    void init();
    
};

typedef struct App42PushNotification
{
    string message;
    string userName;
    string type;
    string deviceToken;
    string expiry;
    vector<App42Channel> channelArray;
    
}App42PushNotification;

typedef struct App42Channel
{
    string channelName;
    string description;
    
}App42Channel;

#endif /* defined(__App42CPPSDK__App42PushNotificationResponse__) */
