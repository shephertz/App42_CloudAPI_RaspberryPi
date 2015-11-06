//
//  Common.h
//  testTool
//
//  Created by Dhruv Chopra on 4/4/13.
//  Copyright (c) 2013 Dhruv Chopra. All rights reserved.
//

#ifndef Common_h
#define Common_h

#include "cJSON.h"
#include "Signing.h"
#include <stdarg.h>

using namespace std;

extern bool isTraceEnabled;

namespace Util {
    
    
    static string getJSONString(string name,cJSON *json)
    {
        std::string value;
        
        json = json->child;
        while(json != NULL)
        {
            if((json->string!=NULL) && (strcmp(json->string,name.c_str()) == 0))
            {
				if(json->valuestring != NULL)
				{
					value = json->valuestring;
					break;
				}
            }
            json = json->next;
        }
        return value;
    }
    
    static string getJSONKeyString(cJSON *json)
    {
        std::string key;
        
        json = json->child;
        while(json != NULL)
        {
            if(json->string!=NULL)
            {
                key = json->string;
                break;
            }
            json = json->next;
        }
        return key;
    }
	
    static double getJSONDouble(string name,cJSON *json)
    {
        double value = 0;
        
        json = json->child;
        while(json != NULL)
        {
            if((json->string!=NULL) && (strcmp(json->string,name.c_str()) ==  0))
            {
                value = json->valuedouble;
                break;
            }
            json = json->next;
        }
        return value;
    }
    
    static int getJSONInt(string name, cJSON *json)
    {
        int value = 0;
        
        json = json->child;
        while(json != NULL)
        {
            if((json->string!=NULL) && (strcmp(json->string,name.c_str()) ==  0))
            {
                value = json->valueint;
                break;
            }
            json = json->next;
        }
        return value;
        
    }
    
    static int getJSONBool(string name, cJSON *json)
    {
        int value = 0;
        
        json = json->child;
        while(json != NULL)
        {
            if((json->string!=NULL) && (strcmp(json->string,name.c_str()) ==  0))
            {
                value = json->valueint;
                break;
            }
            json = json->next;
        }
        return value;
        
    }
    
    static cJSON* getJSONChild(string name,cJSON *json)
    {
        json = json->child;
        while(json != NULL)
        {
            if((json->string!=NULL) && (strcmp(json->string,name.c_str()) ==  0))
            {
                return json;
            }
            json = json->next;
        }
		
        return NULL;
    }
    
    static string getErrorDetails(string errorBody)
    {
        cJSON *ptrJson = cJSON_Parse(errorBody.c_str());
        cJSON* ptrFault = Util::getJSONChild("app42Fault", ptrJson);
        string detailString = Util::getJSONString("details", ptrFault);
        cJSON_Delete(ptrJson);
        return detailString;
    }
    
    
    
    static void app42Trace( const char* format, ... )
    {
        if (isTraceEnabled)
        {            
            va_list args;
            fprintf( stderr, "App42Trace: " );
            va_start( args, format );
            vfprintf( stderr, format, args );
            va_end( args );
            fprintf( stderr, "\n" );
        }
    }

}

#endif
