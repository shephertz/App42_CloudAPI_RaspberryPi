//
//  App42HttpRequest.h
//  App42HttpSample
//
//  Created by Rajeev Ranjan on 09/04/14.
//
//

#ifndef App42HttpSample_App42HttpRequest_h
#define App42HttpSample_App42HttpRequest_h

#include <string>
#include <vector>
#include "App42CallBack.h"
#include <map>

namespace App42Network {
    
    class App42HttpClient;
    class App42HttpResponse;
    //class App42Response;
    
//    typedef void (App42CallBack:: *SEL_App42Response)(App42HttpClient* client, App42HttpResponse* response);
//    #define app42response_selector(_SELECTOR) (App42Network::SEL_App42Response)(&_SELECTOR)
    typedef std::function<void(App42HttpClient* client, App42HttpResponse* response)> SEL_App42Response;
    
    
    /**
     @brief defines the object which users must packed for HttpClient::send(HttpRequest*) method.
     Please refer to tests/test-cpp/Classes/ExtensionTest/NetworkTest/HttpClientTest.cpp as a sample
     @since v2.0.2
     */
    
    class App42HttpRequest
    {
    public:
        /** Use this enum type as param in setReqeustType(param) */
        enum App42RequestType
        {
            App42_GET,
            App42_POST,
            App42_PUT,
            App42_DELETE,
            App42_POSTFILE,
            App42_POSTFILEDATA,
            App42_UNKNOWN,
        };
        
        /** Constructor
         Because HttpRequest object will be used between UI thead and network thread,
         requestObj->autorelease() is forbidden to avoid crashes in AutoreleasePool
         new/retain/release still works, which means you need to release it manually
         Please refer to HttpRequestTest.cpp to find its usage
         */
        App42HttpRequest()
        {
            _requestType = App42_UNKNOWN;
            _url.clear();
            _requestData.clear();
            _tag.clear();
            _pUserData = NULL;
        };
        
        /** Destructor */
        virtual ~App42HttpRequest()
        {

        };
        
//        /** Override autorelease method to avoid developers to call it */
//        Ref* autorelease(void)
//        {
//            CCASSERT(false, "HttpResponse is used between network thread and ui thread \
//                     therefore, autorelease is forbidden here");
//            return NULL;
//        }
        
        // setter/getters for properties
        
        /** Required field for HttpRequest object before being sent.
         kHttpGet & kHttpPost is currently supported
         */
        inline void setHttpRequestType(App42RequestType type)
        {
            _requestType = type;
        };
        /** Get back the kHttpGet/Post/... enum value */
        inline App42RequestType getHttpRequestType()
        {
            return _requestType;
        };
        
        /** Required field for HttpRequest object before being sent.
         */
        inline void setUrl(const char* url)
        {
            _url = url;
        };
        /** Get back the setted url */
        inline const char* getUrl()
        {
            return _url.c_str();
        };
        
        /** Option field. You can set your post data here
         */
        inline void setRequestData(const char* buffer, unsigned int len)
        {
            _requestData.assign(buffer, buffer + len);
        };
        /** Get the request data pointer back */
        inline char* getRequestData()
        {
            if(_requestData.size() != 0)
                return &(_requestData.front());
            
            return nullptr;
        }
        /** Get the size of request data back */
        inline long getRequestDataSize()
        {
            return _requestData.size();
        }
        
        /** Option field. You can set your file data here
         */
        inline void setFileData(unsigned char* buffer, unsigned int len)
        {
            _fileData.assign(buffer, buffer + len);
        };
        /** Get the file data pointer back */
        inline char* getFileData()
        {
            if(_fileData.size() != 0)
                return &(_fileData.front());
            
            return nullptr;
        }
        /** Get the size of file data back */
        inline long getFileDataSize()
        {
            return _fileData.size();
        }

        
        /** Option field. You can set a string tag to identify your request, this tag can be found in HttpResponse->getHttpRequest->getTag()
         */
        inline void setTag(const char* tag)
        {
            _tag = tag;
        };
        /** Get the string tag back to identify the request.
         The best practice is to use it in your MyClass::onMyHttpRequestCompleted(sender, HttpResponse*) callback
         */
        inline const char* getTag()
        {
            return _tag.c_str();
        };
        
        /** Option field. You can attach a customed data in each request, and get it back in response callback.
         But you need to new/delete the data pointer manully
         */
        inline void setUserData(void* pUserData)
        {
            _pUserData = pUserData;
        };
        /** Get the pre-setted custom data pointer back.
         Don't forget to delete it. HttpClient/HttpResponse/HttpRequest will do nothing with this pointer
         */
        inline void* getUserData()
        {
            return _pUserData;
        };
        
        /** Set any custom headers **/
        inline void setHeaders(std::vector<std::string> pHeaders)
        {
            _headers=pHeaders;
        }
        
        /** Get custom headers **/
        inline std::vector<std::string> getHeaders()
        {
            return _headers;
        }

        
        inline void setResponseCallback(SEL_App42Response pSelector)
        {
            _pSelector = pSelector;
        }
        /** Get the target of callback selector funtion, mainly used by HttpClient */
        inline App42CallBack* getTarget()
        {
            return _pTarget;
        }
        
        inline SEL_App42Response getSelector()
        {
            return _pSelector;
        }
        
        void setRequestName(std::string requestName)
        {
            _requestName = requestName;
        }
        
        void setFilePath(std::string filePath)
        {
            _filePath = filePath;
        }
        
        void setFileName(std::string fileName)
        {
            _fileName = fileName;
        }
        
        void setPostParams(std::map<std::string,std::string> postParams)
        {
            _postParams = postParams;
        }
        
        inline int dataSize()
        {
            return _dataSize;
        }
        
        inline std::string fileName()
        {
            return _fileName;
        }
        
        inline std::string filePath()
        {
            return _filePath;
        }
        
        inline std::string requestName()
        {
            return _requestName;
        }
        
        inline std::map<std::string,std::string> postParams()
        {
            return _postParams;
        }
        
    protected:
        // properties
        App42RequestType            _requestType;    /// kHttpRequestGet, kHttpRequestPost or other enums
        std::string                 _url;            /// target url that this request is sent to
        std::vector<char>           _requestData;    /// used for POST
        std::vector<char>           _fileData;       /// used for POST
        std::string                 _tag;            /// user defined tag, to identify different requests in response callback
        App42CallBack*              _pTarget;        /// callback target of pSelector function
        SEL_App42Response           _pSelector;      /// callback function, e.g. MyLayer::onHttpResponse(HttpClient *sender, HttpResponse *                                     response)
        void*                       _pUserData;      /// You can add your customed data here 
        std::vector<std::string>    _headers;		 /// custom http headers
        int                         _dataSize;        // File data size
        std::string                _fileName;        // File name to be saved
        std::string                 _filePath;        // File Path of file that need to be uploaded
        std::string                _requestName;     // name of upload type such as "uploadFile" etc.
        std::map<std::string,std::string> _postParams;
    };
    
}


#endif
