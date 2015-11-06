//
//  App42HttpClient.h
//  App42HttpSample
//
//  Created by Rajeev Ranjan on 09/04/14.
//
//

#ifndef __App42HttpSample__App42HttpClient__
#define __App42HttpSample__App42HttpClient__

#include <iostream>
#include "App42HttpRequest.h"
#include "App42HttpResponse.h"


namespace App42Network
{
    
    /** @brief Singleton that handles asynchrounous http requests
     * Once the request completed, a callback will issued in main thread when it provided during make request
     */
    class App42HttpClient
    {
    public:
        /** Return the shared instance **/
        static App42HttpClient *getInstance();
        
        /** Relase the shared instance **/
        static void destroyInstance();
        
        /** Enable cookie support. **/
        void enableCookies(const char* cookieFile);
        
        /**
         * Add a get request to task queue
         * @param request a HttpRequest object, which includes url, response callback etc.
         please make sure request->_requestData is clear before calling "send" here.
         */
        void send(App42HttpRequest* request);
        
        /**
         * Change the connect timeout
         * @param value The desired timeout.
         */
        inline void setTimeoutForConnect(int value) {_timeoutForConnect = value;};
        
        /**
         * Get connect timeout
         * @return int
         */
        inline int getTimeoutForConnect() {return _timeoutForConnect;}
        
        
        /**
         * Change the download timeout
         * @param value
         */
        inline void setTimeoutForRead(int value) {_timeoutForRead = value;};
        
        
        /**
         * Get download timeout
         * @return int
         */
        inline int getTimeoutForRead() {return _timeoutForRead;};
        
    private:
        App42HttpClient();
        virtual ~App42HttpClient();
        bool init(void);
        
        /**
         * Init pthread mutex, semaphore, and create new thread for http requests
         * @return bool
         */
        bool lazyInitThreadSemphore();
        void networkThread();
        /** Poll function called from main thread to dispatch callbacks when http requests finished **/
        void dispatchResponseCallbacks();
        
    private:
        int _timeoutForConnect;
        int _timeoutForRead;
    };
    
}

#endif /* defined(__App42HttpSample__App42HttpClient__) */
