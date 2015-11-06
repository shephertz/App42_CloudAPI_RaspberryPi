//
//  App42EventResponse.cpp
//  App42Cocos2dXSample
//
//  Created by Rajeev Ranjan on 13/07/15.
//
//

#include "App42EventResponse.h"
#include "Common.h"


App42EventResponse::App42EventResponse(const app42CallBack& pSelector)
:App42Response(pSelector)
{
    
}

App42EventResponse::~App42EventResponse()
{
    
}

void App42EventResponse::onComplete(void *sender, void *data)
{
    App42Response::onComplete(sender, data);
    init();
    
    if (_app42Selector)
    {
        _app42Selector(this);
    }
}

void App42EventResponse::init()
{
    if(_result != 200)
    {
        Util::app42Trace("\nApp42EvenServiceRequest failed with result code : %d", _result);
        buildErrorMessage();
    }
}