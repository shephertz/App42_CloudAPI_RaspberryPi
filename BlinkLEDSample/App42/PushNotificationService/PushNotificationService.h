//
//  PushNotificationService.h
//  App42CPPSDK
//
//  Created by Rajeev Ranjan on 21/10/13.
//
//

#ifndef __App42CPPSDK__PushNotificationService__
#define __App42CPPSDK__PushNotificationService__

#include <iostream>
#include "App42Service.h"

using namespace std;

typedef enum DeviceType
{
    APP42_IOS,
    APP42_ANDROID,
    APP42_WP7,
    APP42_NOKIAX,
}DeviceType;

class PushNotificationService : public App42Service
{
    
private:
    PushNotificationService();
	static PushNotificationService* _instance;
    
    inline const char *getDeviceType(DeviceType deviceType)
    {
        static const char *strings[] = {"iOS", "ANDROID", "WP7","NokiaX"};
        
        return strings[deviceType];
    }
    
public:
    
    
    
	/**
     * This is a constructor that takes
     *
     * @param apiKey
     * @param secretKey
     * @param server
     *
     */
    static PushNotificationService* Initialize(string apikey, string secretkey);
    
	static void Terminate();
    
	/**
     * Builds the instance of PushNotificationService.
     *
     * @return PushNotificationService - PushNotificationService Object
     */
	static PushNotificationService* getInstance();
    
    
    void CreateChannel(const char* channel,const char* description,const app42CallBack& pSelector);
    
    void RegisterDeviceToken(const char* deviceToken, const char* userName, DeviceType deviceType, const app42CallBack& pSelector);
    
    void SendPushMessageToUser(const char* username,  const char* message, const app42CallBack& pSelector);
    
    void SendPushMessageToUser(const char* userName, map<string, string>messageMap, const app42CallBack& pSelector);
    
    void SendPushMessageToAll(const char* message, const app42CallBack& pSelector);
    
    void SubscribeToChannel(const char* channel, const char* userName, const app42CallBack& pSelector);
    
    void SubscribeToChannel(const char* userName,const char* channelName, const char* deviceToken, DeviceType deviceType, const app42CallBack& pSelector);
    
    void UnsubscribeFromChannel(const char* channel, const char* userName, const app42CallBack& pSelector);
    
    void UnsubscribeDeviceToChannel(const char* userName,const char* channelName, const char* deviceToken, const app42CallBack& pSelector);
    
    void SendPushMessageToChannel(const char* channel, const char* message, const app42CallBack& pSelector);
    
    void SendPushMessageToChannel(const char* channel, map<string, string>messageMap, const app42CallBack& pSelector);

    void DeleteDeviceToken(const char* deviceToken, const char* userName, const app42CallBack& pSelector);
    void DeleteAllDevices(const char* userName, const app42CallBack& pSelector);
    
    void UnsubscribeDevice(const char* deviceToken, const char* userName, const app42CallBack& pSelector);
    void ResubscribeDevice(const char* deviceToken, const char* userName, const app42CallBack& pSelector);
    
    void SendPushMessageToAllByType(const char* message,DeviceType deviceType, const app42CallBack& pSelector);
    
    /**
     * Send push message to list of specific users.
     *
     * @param message
     *            - Message which you have to send.
     * @param userList
     *            - List of the users for which message has to be send.
     * @return PushNotification Object
     * @throws App42Exception
     */
    void SendPushMessageToGroup(vector<string> userList,const char* message, const app42CallBack& pSelector);
    
    void SendPushToTargetUsers(const char* message,const char* dbName, const char* collectionName, Query *query, const app42CallBack& pSelector);
    
    void ScheduleMessageToUser(const char* username,const char* message, tm *expiryDate,const app42CallBack& pSelector);
    
    void SendPushMessageToDevice(const char* deviceToken, const char* username,  const char* message, const app42CallBack& pSelector);
    
};
#endif /* defined(__App42CPPSDK__PushNotificationService__) */
