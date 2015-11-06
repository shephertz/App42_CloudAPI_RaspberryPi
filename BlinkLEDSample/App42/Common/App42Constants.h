//
//  App42Constants.h
//  App42CPPSDK
//
//  Created by Rajeev Ranjan on 08/10/13.
//
//

#ifndef App42CPPSDK_App42Constants_h
#define App42CPPSDK_App42Constants_h


#define Production 1

#if Production
    #define PROTOCOL        "https://"
    #define BASE_URL        "api.shephertz.com"
#else
    #define PROTOCOL        "http://"
    #define BASE_URL        "192.168.1.39:8082/App42_API_SERVER"
#endif

#define CUSTOMCODE_URL  "customcode.shephertz.com/"
#define APP42_ANALYTICS_URL "analytics.shephertz.com"
#define RESOURCE_BASE       "/cloud/1.0/"
#define VERSION             "1.0"

/***
 *  BASIC SERVICE PARAMS
 **/

#define VERSION_KEY         "version"
#define TIME_SATMP          "timeStamp"
#define LOG_TAG             "App42"
#define SESSION_ID          "sessionId"
#define ADMIN_KEY           "adminKey"
#define PAGE_OFFSET         "offset"
#define PAGE_MAX_RECORDS    "max"
#define DATA_App42ACL_HEADER     "dataApp42ACL"
#define SELECT_KEY_FLAG     "1"
#define SELECT_KEYS_HEADER  "selectKeys"
#define FB_ACCESS_TOKEN     "fbAccessToken"
#define GEO_TAG             "geoTag"
#define EVENT               "event"

#define APP42_APPSTATETRACKING_ENABLED  @"app42AppStateTrackingEnabled"
#define APP42_EVENTSERVICE_ENABLED      @"app42EventServiceEnabled"

#endif
