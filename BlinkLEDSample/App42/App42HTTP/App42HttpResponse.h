//
//  App42HttpResponse.h
//  App42HttpSample
//
//  Created by Rajeev Ranjan on 09/04/14.
//
//

#ifndef App42HttpSample_App42HttpResponse_h
#define App42HttpSample_App42HttpResponse_h

#include "App42HttpRequest.h"


namespace App42Network
{
    
    /**
     @brief defines the object which users will receive at onHttpCompleted(sender, HttpResponse) callback
     Please refer to samples/TestCpp/Classes/ExtensionTest/NetworkTest/HttpClientTest.cpp as a sample
     @since v2.0.2
     */
    class App42HttpResponse
    {
    public:
        /** Constructor, it's used by HttpClient internal, users don't need to create HttpResponse manually
         @param request the corresponding HttpRequest which leads to this response
         */
        App42HttpResponse(App42HttpRequest* request)
        {
            _pHttpRequest = request;
            if (_pHttpRequest)
            {
                //_pHttpRequest->retain();
            }
            
            _succeed = false;
            _responseData.clear();
            _errorBuffer.clear();
        }
        
        /** Destructor, it will be called in HttpClient internal,
         users don't need to desturct HttpResponse object manully
         */
        virtual ~App42HttpResponse()
        {
            if (_pHttpRequest)
            {
                //_pHttpRequest->release();
            }
        }
        
//        /** Override autorelease method to prevent developers from calling it */
//        cocos2d::Ref* autorelease(void)
//        {
//            CCASSERT(false, "HttpResponse is used between network thread and ui thread \
//                     therefore, autorelease is forbidden here");
//            return NULL;
//        }
        
        // getters, will be called by users
        
        /** Get the corresponding HttpRequest object which leads to this response
         There's no paired setter for it, coz it's already setted in class constructor
         */
        inline App42HttpRequest* getApp42HttpRequest()
        {
            return _pHttpRequest;
        }
        
        /** To see if the http reqeust is returned successfully,
         Althrough users can judge if (http return code = 200), we want an easier way
         If this getter returns false, you can call getResponseCode and getErrorBuffer to find more details
         */
        inline bool isSucceed()
        {
            return _succeed;
        };
        
        /** Get the http response raw data */
        inline std::vector<char>* getResponseData()
        {
            return &_responseData;
        }
        
        /** get the Rawheader **/
        inline std::vector<char>* getResponseHeader()
        {
            return &_responseHeader;
        }
        
        /** Get the http response errorCode
         *  I know that you want to see http 200 :)
         */
        inline long getResponseCode()
        {
            return _responseCode;
        }
        
        /** Get the rror buffer which will tell you more about the reason why http request failed
         */
        inline const char* getErrorBuffer()
        {
            return _errorBuffer.c_str();
        }
        
        // setters, will be called by HttpClient
        // users should avoid invoking these methods
        
        
        /** Set if the http request is returned successfully,
         Althrough users can judge if (http code == 200), we want a easier way
         This setter is mainly used in HttpClient, users mustn't set it directly
         */
        inline void setSucceed(bool value)
        {
            _succeed = value;
        };
        
        
        /** Set the http response raw buffer, is used by HttpClient
         */
        inline void setResponseData(std::vector<char>* data)
        {
            _responseData = *data;
        }
        
        /** Set the http response Header raw buffer, is used by HttpClient
         */
        inline void setResponseHeader(std::vector<char>* data)
        {
            _responseHeader = *data;
        }
        
        
        /** Set the http response errorCode
         */
        inline void setResponseCode(long value)
        {
            _responseCode = value;
        }
        
        
        /** Set the error buffer which will tell you more the reason why http request failed
         */
        inline void setErrorBuffer(const char* value)
        {
            _errorBuffer.clear();
            _errorBuffer.assign(value);
        };
        
    protected:
        bool initWithRequest(App42HttpRequest* request);
        
        // properties
        App42HttpRequest*   _pHttpRequest;  /// the corresponding HttpRequest pointer who leads to this response
        bool                _succeed;       /// to indecate if the http reqeust is successful simply
        std::vector<char>   _responseData;  /// the returned raw data. You can also dump it as a string
        std::vector<char>   _responseHeader;  /// the returned raw header data. You can also dump it as a string
        long                _responseCode;    /// the status code returned from libcurl, e.g. 200, 404
        std::string         _errorBuffer;   /// if _responseCode != 200, please read _errorBuffer to find the reason 
        
    };
    
}


#endif
