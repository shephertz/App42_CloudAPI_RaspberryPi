//
//  App42EventResponse.h
//  App42Cocos2dXSample
//
//  Created by Rajeev Ranjan on 13/07/15.
//
//

#ifndef __App42Cocos2dXSample__App42EventResponse__
#define __App42Cocos2dXSample__App42EventResponse__

#include <stdio.h>
#include "App42Response.h"

using namespace std;

class App42EventResponse : public App42Response
{
public:
    
    App42EventResponse(const app42CallBack& pSelector);
    ~App42EventResponse();
    void onComplete(void *sender, void *data);
    
private:
    
    void init();
    
};

#endif /* defined(__App42Cocos2dXSample__App42EventResponse__) */
