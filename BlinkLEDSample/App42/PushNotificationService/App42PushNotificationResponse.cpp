//
//  App42PushNotificationResponse.cpp
//  App42CPPSDK
//
//  Created by Rajeev Ranjan on 21/10/13.
//
//

#include "App42PushNotificationResponse.h"

#include "Common.h"


App42PushNotificationResponse::App42PushNotificationResponse(const app42CallBack& pSelector)
:App42Response(pSelector)
{
    
}

App42PushNotificationResponse::~App42PushNotificationResponse()
{
    
}

void App42PushNotificationResponse::onComplete(void *sender, void *data)
{
    App42Response::onComplete(sender, data);
    init();
    
    
    if (_app42Selector)
    {
        _app42Selector(this);
    }
    
}

void App42PushNotificationResponse::init()
{
    if(_result != 200)
    {
        Util::app42Trace("App42PushNotification failed result is %d", _result);
        buildErrorMessage();
        return;
    }
    // parse the body
    cJSON *ptrBody = cJSON_Parse(_body.c_str());
    cJSON* ptrApp42 = Util::getJSONChild("app42", ptrBody);
    cJSON* ptrResponse = Util::getJSONChild("response", ptrApp42);
    cJSON* ptrPush = Util::getJSONChild("push", ptrResponse);
    
    
    cJSON* child = ptrPush;
    if(child->type == cJSON_Array)
    {
        child = child->child;
    }
    
    while(child != NULL && child->type == cJSON_Object)
    {
        App42PushNotification app42PushNotification;
        app42PushNotification.userName = Util::getJSONString("userName", child);
        
        cJSON* ptrMessage = Util::getJSONChild("message", child);
        if (ptrMessage && ptrMessage->type == cJSON_Object)
        {
           // app42PushNotification.message = Util::getJSONString("message", child);
           app42PushNotification.message = cJSON_PrintUnformatted(ptrMessage);
        }
        else
        {
            app42PushNotification.message = Util::getJSONString("message", child);
        }
        app42PushNotification.type = Util::getJSONString("type", child);
        app42PushNotification.deviceToken = Util::getJSONString("deviceToken", child);
        app42PushNotification.expiry = Util::getJSONString("expiry", child);

        cJSON* ptrChannels = Util::getJSONChild("channels", child);
        if (ptrChannels)
        {
            cJSON* ptrChannel = Util::getJSONChild("channel", ptrChannels);
            if (ptrChannel)
            {
                cJSON* child = ptrChannel;
                if(child->type == cJSON_Array)
                {
                    child = child->child;
                }
                while(child != NULL && child->type == cJSON_Object)
                {
                    App42Channel channel;
                    channel.channelName = Util::getJSONString("channelName", ptrChannel);
                    channel.description = Util::getJSONString("description", ptrChannel);
                    app42PushNotification.channelArray.push_back(channel);
                    child = child->next;
                }

                
            }
        }
        notifications.push_back(app42PushNotification);
        child = child->next;
    }
    
    cJSON_Delete(ptrBody);
}
