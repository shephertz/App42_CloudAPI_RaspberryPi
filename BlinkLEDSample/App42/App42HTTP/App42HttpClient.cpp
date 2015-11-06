//
//  App42HttpClient.cpp
//  App42HttpSample
//
//  Created by Rajeev Ranjan on 09/04/14.
//
//

#include "App42HttpClient.h"
#include <thread>
#include <queue>
#include <condition_variable>
#include <vector>
#include <errno.h>

#include "curl/curl.h"



namespace App42Network
{
    
    static std::mutex       s_requestQueueMutex;
    static std::mutex       s_responseQueueMutex;
    
    static std::mutex		s_SleepMutex;
    static std::condition_variable		s_SleepCondition;
    
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    typedef int int32_t;
#endif
    
    static bool s_need_quit = false;
    
    static std::vector<App42HttpRequest*>*  s_requestQueue = nullptr;
    static std::vector<App42HttpResponse*>* s_responseQueue = nullptr;
    
    static App42HttpClient *s_pHttpClient = nullptr; // pointer to singleton
    
    static char s_errorBuffer[CURL_ERROR_SIZE] = {0};
    
    typedef size_t (*write_callback)(void *ptr, size_t size, size_t nmemb, void *stream);
    
    static std::string s_cookieFilename = "";
    
    // Callback function used by libcurl for collect response data
    static size_t writeData(void *ptr, size_t size, size_t nmemb, void *stream)
    {
        std::vector<char> *recvBuffer = (std::vector<char>*)stream;
        size_t sizes = size * nmemb;
        
        // add data to the end of recvBuffer
        // write data maybe called more than once in a single request
        recvBuffer->insert(recvBuffer->end(), (char*)ptr, (char*)ptr+sizes);
        
        return sizes;
    }
    
    // Callback function used by libcurl for collect header data
    static size_t writeHeaderData(void *ptr, size_t size, size_t nmemb, void *stream)
    {
        std::vector<char> *recvBuffer = (std::vector<char>*)stream;
        size_t sizes = size * nmemb;
        
        // add data to the end of recvBuffer
        // write data maybe called more than once in a single request
        recvBuffer->insert(recvBuffer->end(), (char*)ptr, (char*)ptr+sizes);
        
        return sizes;
    }
    
    
    static int processGetTask(App42HttpRequest *request, write_callback callback, void *stream, long *errorCode, write_callback headerCallback, void *headerStream);
    static int processPostTask(App42HttpRequest *request, write_callback callback, void *stream, long *errorCode, write_callback headerCallback, void *headerStream);
    static int processPutTask(App42HttpRequest *request, write_callback callback, void *stream, long *errorCode, write_callback headerCallback, void *headerStream);
    static int processDeleteTask(App42HttpRequest *request, write_callback callback, void *stream, long *errorCode, write_callback headerCallback, void *headerStream);
    static int processPostFileTask(App42HttpRequest *request, write_callback callback, void *stream, long *responseCode, write_callback headerCallback, void *headerStream);
    static int processPostFileDataTask(App42HttpRequest *request, write_callback callback, void *stream, long  *responseCode, write_callback headerCallback, void *headerStream);
    // int processDownloadTask(App42HttpRequest *task, write_callback callback, void *stream, int32_t *errorCode);
    
    
    // Worker thread
    void App42HttpClient::networkThread()
    {
        App42HttpRequest *request = nullptr;
        
        //auto scheduler = cocos2d::Director::getInstance()->getScheduler();
        
        while (true)
        {
            if (s_need_quit)
            {
                break;
            }
            
            // step 1: send http request if the requestQueue isn't empty
            request = nullptr;
            
            s_requestQueueMutex.lock();
            
            //Get request task from queue
            
            if (!s_requestQueue->empty())
            {
                std::vector<App42HttpRequest*>::iterator it= s_requestQueue->begin();
                request = *it;
                s_requestQueue->erase(it);
            }
            
            s_requestQueueMutex.unlock();
            
            if (nullptr == request)
            {
                // Wait for http request tasks from main thread
                std::unique_lock<std::mutex> lk(s_SleepMutex);
                s_SleepCondition.wait(lk);
                continue;
            }
            
            // step 2: libcurl sync access
            
            // Create a HttpResponse object, the default setting is http access failed
            App42HttpResponse *response = new App42HttpResponse(request);
            
            // request's refcount = 2 here, it's retained by HttpRespose constructor
                //request->release();
            // ok, refcount = 1 now, only HttpResponse hold it.
            
            long responseCode = -1;
            int retValue = 0;
            
            // Process the request -> get response packet
            switch (request->getHttpRequestType())
            {
                case App42HttpRequest::App42_GET: // HTTP GET
                    retValue = processGetTask(request,
                                              writeData,
                                              response->getResponseData(),
                                              &responseCode,
                                              writeHeaderData,
                                              response->getResponseHeader());
                    break;
                    
                case App42HttpRequest::App42_POST: // HTTP POST
                    retValue = processPostTask(request,
                                               writeData,
                                               response->getResponseData(),
                                               &responseCode,
                                               writeHeaderData,
                                               response->getResponseHeader());
                    break;
                    
                case App42HttpRequest::App42_PUT:
                    retValue = processPutTask(request,
                                              writeData,
                                              response->getResponseData(),
                                              &responseCode,
                                              writeHeaderData,
                                              response->getResponseHeader());
                    break;
                    
                case App42HttpRequest::App42_DELETE:
                    retValue = processDeleteTask(request,
                                                 writeData,
                                                 response->getResponseData(),
                                                 &responseCode,
                                                 writeHeaderData,
                                                 response->getResponseHeader());
                    break;
                    
                case App42HttpRequest::App42_POSTFILE:
                    retValue = processPostFileTask(request,
                                                 writeData,
                                                 response->getResponseData(),
                                                 &responseCode,
                                                 writeHeaderData,
                                                 response->getResponseHeader());
                    break;
                case App42HttpRequest::App42_POSTFILEDATA:
                    retValue = processPostFileDataTask(request,
                                                   writeData,
                                                   response->getResponseData(),
                                                   &responseCode,
                                                   writeHeaderData,
                                                   response->getResponseHeader());
                    break;
                default:
                    //CCASSERT(true, "App42HttpClient: unkown request type, only GET, POST, PUT, DELETE, POSTFILEDATA and POSTFILE are supported");
                    break;
            }
            
            // write data to HttpResponse
            response->setResponseCode(responseCode);
            
            if (retValue != 0)
            {
                response->setSucceed(false);
                response->setErrorBuffer(s_errorBuffer);
            }
            else
            {
                response->setSucceed(true);
            }
            
            //printf("ResponseCode=%ld",response->getResponseCode());
            // add response packet into queue
            s_responseQueueMutex.lock();
            s_responseQueue->push_back(response);
            s_responseQueueMutex.unlock();
            
            if (nullptr != s_pHttpClient) {
                //scheduler->performFunctionInCocosThread(CC_CALLBACK_0(App42HttpClient::dispatchResponseCallbacks, this));
                dispatchResponseCallbacks();
            }
        }
        
        // cleanup: if worker thread received quit signal, clean up un-completed request queue
        s_requestQueueMutex.lock();
        s_requestQueue->clear();
        s_requestQueueMutex.unlock();
        
        
        if (s_requestQueue != nullptr) {
            delete s_requestQueue;
            s_requestQueue = nullptr;
            delete s_responseQueue;
            s_responseQueue = nullptr;
        }
        
    }
    
    //Configure curl's timeout property
    static bool configureCURL(CURL *handle)
    {
        if (!handle) {
            return false;
        }
        
        int32_t code;
        code = curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, s_errorBuffer);
        if (code != CURLE_OK) {
            return false;
        }
        code = curl_easy_setopt(handle, CURLOPT_TIMEOUT, App42HttpClient::getInstance()->getTimeoutForRead());
        if (code != CURLE_OK) {
            return false;
        }
        code = curl_easy_setopt(handle, CURLOPT_CONNECTTIMEOUT, App42HttpClient::getInstance()->getTimeoutForConnect());
        if (code != CURLE_OK) {
            return false;
        }
        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0L);
        
        
        // FIXED #3224: The subthread of CCHttpClient interrupts main thread if timeout comes.
        // Document is here: http://curl.haxx.se/libcurl/c/curl_easy_setopt.html#CURLOPTNOSIGNAL
        curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1L);
        
        return true;
    }
    
    class CURLRaii
    {
        /// Instance of CURL
        CURL *_curl;
        /// Keeps custom header data
        curl_slist *_headers;
    public:
        CURLRaii()
        : _curl(curl_easy_init())
        , _headers(nullptr)
        {
        }
        
        ~CURLRaii()
        {
            if (_curl)
                curl_easy_cleanup(_curl);
            /* free the linked list for header data */
            if (_headers)
                curl_slist_free_all(_headers);
        }
        
        template <class T>
        bool setOption(CURLoption option, T data)
        {
            return CURLE_OK == curl_easy_setopt(_curl, option, data);
        }
        
        /**
         * @brief Inits CURL instance for common usage
         * @param request Null not allowed
         * @param callback Response write callback
         * @param stream Response write stream
         */
        bool init(App42HttpRequest *request, write_callback callback, void *stream, write_callback headerCallback, void *headerStream)
        {
            if (!_curl)
                return false;
            if (!configureCURL(_curl))
                return false;
            
            /* get custom header data (if set) */
            std::vector<std::string> headers=request->getHeaders();
            if(!headers.empty())
            {
                /* append custom headers one by one */
                for (std::vector<std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
                    _headers = curl_slist_append(_headers,it->c_str());
                /* set custom headers for curl */
                if (!setOption(CURLOPT_HTTPHEADER, _headers))
                    return false;
            }
            if (!s_cookieFilename.empty()) {
                if (!setOption(CURLOPT_COOKIEFILE, s_cookieFilename.c_str())) {
                    return false;
                }
                if (!setOption(CURLOPT_COOKIEJAR, s_cookieFilename.c_str())) {
                    return false;
                }
            }
            
            return setOption(CURLOPT_URL, request->getUrl())
            && setOption(CURLOPT_WRITEFUNCTION, callback)
            && setOption(CURLOPT_WRITEDATA, stream)
            && setOption(CURLOPT_HEADERFUNCTION, headerCallback)
            && setOption(CURLOPT_HEADERDATA, headerStream);
            
        }
        
        /// @param responseCode Null not allowed
        bool perform(long *responseCode)
        {
            if (CURLE_OK != curl_easy_perform(_curl))
                return false;
            CURLcode code = curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, responseCode);
            if (code != CURLE_OK || *responseCode != 200) {
                printf("Curl curl_easy_getinfo failed: %s", curl_easy_strerror(code));
                return false;
            }
            // Get some more data.
            
            return true;
        }
    };
    
    //Process Get Request
    static int processGetTask(App42HttpRequest *request, write_callback callback, void *stream, long *responseCode, write_callback headerCallback, void *headerStream)
    {
        CURLRaii curl;
        bool ok = curl.init(request, callback, stream, headerCallback, headerStream)
        && curl.setOption(CURLOPT_FOLLOWLOCATION, true)
        && curl.perform(responseCode);
        return ok ? 0 : 1;
    }
    
    //Process POST Request
    static int processPostTask(App42HttpRequest *request, write_callback callback, void *stream, long *responseCode, write_callback headerCallback, void *headerStream)
    {
        CURLRaii curl;
        bool ok = curl.init(request, callback, stream, headerCallback, headerStream)
        && curl.setOption(CURLOPT_POST, 1)
        && curl.setOption(CURLOPT_POSTFIELDS, request->getRequestData())
        && curl.setOption(CURLOPT_POSTFIELDSIZE, request->getRequestDataSize())
        && curl.perform(responseCode);
        return ok ? 0 : 1;
    }
    
    //Process PUT Request
    static int processPutTask(App42HttpRequest *request, write_callback callback, void *stream, long *responseCode, write_callback headerCallback, void *headerStream)
    {
        CURLRaii curl;
        bool ok = curl.init(request, callback, stream, headerCallback, headerStream)
        && curl.setOption(CURLOPT_CUSTOMREQUEST, "PUT")
        && curl.setOption(CURLOPT_POSTFIELDS, request->getRequestData())
        && curl.setOption(CURLOPT_POSTFIELDSIZE, request->getRequestDataSize())
        && curl.perform(responseCode);
        return ok ? 0 : 1;
    }
    
    //Process DELETE Request
    static int processDeleteTask(App42HttpRequest *request, write_callback callback, void *stream, long *responseCode, write_callback headerCallback, void *headerStream)
    {
        CURLRaii curl;
        bool ok = curl.init(request, callback, stream, headerCallback, headerStream)
        && curl.setOption(CURLOPT_CUSTOMREQUEST, "DELETE")
        && curl.setOption(CURLOPT_FOLLOWLOCATION, true)
        && curl.perform(responseCode);
        return ok ? 0 : 1;
    }
    
    static int processPostFileTask(App42HttpRequest *request, write_callback callback, void *stream, long  *responseCode, write_callback headerCallback, void *headerStream)
    {
        
        struct curl_httppost *formpost = NULL;
        struct curl_httppost *lastptr = NULL;
        
        std::string fileName = request->fileName();
        std::string filePath = request->filePath();
        std::string requestName = request->requestName();
        std::map<std::string,std::string> postParams = request->postParams();
        printf("\nrequestName=%s\n",requestName.c_str());
        printf("\nfilePath=%s\n",filePath.c_str());
        printf("\nfileName=%s\n",fileName.c_str());
        
        /* Fill in the file upload field */
        curl_formadd(&formpost,
                     &lastptr,
                     CURLFORM_COPYNAME, requestName.c_str(),
                     CURLFORM_FILE, filePath.c_str(),
                     CURLFORM_END);
        
        /* Fill in the filename field */
        curl_formadd(&formpost,
                     &lastptr,
                     CURLFORM_COPYNAME, "filename",
                     CURLFORM_COPYCONTENTS, fileName.c_str(),
                     CURLFORM_END);
        
        
        std::map<std::string,std::string>::iterator it;
        
        for(it=postParams.begin(); it!=postParams.end(); ++it)
        {
            curl_formadd(&formpost,
                         &lastptr,
                         CURLFORM_PTRNAME, it->first.c_str(),
                         CURLFORM_PTRCONTENTS, it->second.c_str(),
                         CURLFORM_END);
        }
        
        
        CURLRaii curl;
        bool ok = curl.init(request, callback, stream, headerCallback, headerStream)
        && curl.setOption(CURLOPT_POST, 1)
        && curl.setOption(CURLOPT_HTTPPOST, formpost)
        && curl.perform(responseCode);
        
        curl_formfree(formpost);
        return ok ? 0 : 1;
    }
    
    
    static int processPostFileDataTask(App42HttpRequest *request, write_callback callback, void *stream, long  *responseCode, write_callback headerCallback, void *headerStream)
    {
        struct curl_httppost *formpost = NULL;
        struct curl_httppost *lastptr = NULL;
        
        std::string fileName = request->fileName();
        std::string requestName = request->requestName();
        std::map<std::string,std::string> postParams = request->postParams();

        printf("\nrequestName=%s\n",requestName.c_str());
        printf("\nfileName=%s\n",fileName.c_str());
        
        curl_formadd(&formpost,
                     &lastptr,
                     CURLFORM_COPYNAME, requestName.c_str(),
                     CURLFORM_BUFFER, fileName.c_str(),
                     CURLFORM_BUFFERPTR, request->getFileData(),
                     CURLFORM_BUFFERLENGTH, (long)request->getFileDataSize(),
                     CURLFORM_CONTENTTYPE,"multipart/form-data",
                     CURLFORM_END);
        
        /* Fill in the filename field */
        curl_formadd(&formpost,
                     &lastptr,
                     CURLFORM_COPYNAME, "filename",
                     CURLFORM_COPYCONTENTS, fileName.c_str(),
                     CURLFORM_END);
        
        
        std::map<std::string,std::string>::iterator it;
        
        for(it=postParams.begin(); it!=postParams.end(); ++it)
        {
            curl_formadd(&formpost,
                         &lastptr,
                         CURLFORM_PTRNAME, it->first.c_str(),
                         CURLFORM_PTRCONTENTS, it->second.c_str(),
                         CURLFORM_END);
            
        }
        
        
        CURLRaii curl;
        bool ok = curl.init(request, callback, stream, headerCallback, headerStream)
        && curl.setOption(CURLOPT_POST, 1)
        && curl.setOption(CURLOPT_HTTPPOST, formpost)
        && curl.perform(responseCode);
        
        curl_formfree(formpost);
        return ok ? 0 : 1;
    }

    
    
    // HttpClient implementation
    App42HttpClient* App42HttpClient::getInstance()
    {
        if (s_pHttpClient == nullptr) {
            s_pHttpClient = new App42HttpClient();
        }
        
        return s_pHttpClient;
    }
    
    void App42HttpClient::destroyInstance()
    {
        delete s_pHttpClient;
        s_pHttpClient = nullptr;
    }
    
//    void App42HttpClient::enableCookies(const char* cookieFile) {
//        if (cookieFile) {
//            s_cookieFilename = std::string(cookieFile);
//        }
//        else {
//            s_cookieFilename = (FileUtils::getInstance()->getWritablePath() + "cookieFile.txt");
//        }
//    }
    
    App42HttpClient::App42HttpClient()
    : _timeoutForConnect(30)
    , _timeoutForRead(60)
    {
    }
    
    App42HttpClient::~App42HttpClient()
    {
        s_need_quit = true;
        
        if (s_requestQueue != nullptr) {
            s_SleepCondition.notify_one();
        }
        
        s_pHttpClient = nullptr;
    }
    
    //Lazy create semaphore & mutex & thread
    bool App42HttpClient::lazyInitThreadSemphore()
    {
        if (s_requestQueue != nullptr) {
            return true;
        } else {
            
            s_requestQueue = new std::vector<App42HttpRequest*>();
            s_responseQueue = new std::vector<App42HttpResponse*>();
            
            auto t = std::thread(std::bind((&App42HttpClient::networkThread),this));
            //(CC_CALLBACK_0(App42HttpClient::networkThread, this));
            t.detach();
            
            s_need_quit = false;
        }
        
        return true;
    }
    
    //Add a get task to queue
    void App42HttpClient::send(App42HttpRequest* request)
    {    
        if (false == lazyInitThreadSemphore()) 
        {
            return;
        }
        
        if (!request)
        {
            return;
        }
        
        //request->retain();
        
        if (nullptr != s_requestQueue) {
            s_requestQueueMutex.lock();
            s_requestQueue->push_back(request);
            s_requestQueueMutex.unlock();
            
            // Notify thread start to work
            s_SleepCondition.notify_one();
        }
    }
    
    // Poll and notify main thread if responses exists in queue
    void App42HttpClient::dispatchResponseCallbacks()
    {
        // log("CCHttpClient::dispatchResponseCallbacks is running");
        //occurs when cocos thread fires but the network thread has already quited
        if (nullptr == s_responseQueue) {
            return;
        }
        App42HttpResponse* response = nullptr;
        
        s_responseQueueMutex.lock();
        
        if (!s_responseQueue->empty())
        {
            std::vector<App42HttpResponse*>::iterator it= s_responseQueue->begin();
            response = *it;
            it = s_responseQueue->erase(it);
        }

        s_responseQueueMutex.unlock();
        
        if (response)
        {
            App42HttpRequest *request = response->getApp42HttpRequest();
            SEL_App42Response pSelector = request->getSelector();
            
            if (pSelector)
            {
                pSelector(this, response);
            }
            //printf("\nCallBack Sent\n");
            
            if (request) {
                delete request;
                request = nullptr;
            }
            
            if (response)
            {
                delete response;
                response = nullptr;
            }
            
        }
    }
    
}
