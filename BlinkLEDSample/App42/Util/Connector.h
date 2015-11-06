//
//  Connector.h
//  App42CPPSDK
//
//  Created by Rajeev Ranjan on 16/10/13.
//
//

#ifndef App42CPPSDK_Connector_h
#define App42CPPSDK_Connector_h


#include "App42Response.h"
#include "curl/curl.h"
#include "Common.h"
#include "App42HttpRequest.h"
#include "App42HttpResponse.h"
#include "App42HttpClient.h"

using namespace std;


namespace Util
{
    
    static const char* url_encode(string &url)
    {
        CURL *curl = curl_easy_init();
        char *str = curl_easy_escape(curl, url.c_str(), url.size());
        return str;
    }
    
    static void printMap(map<std::string, std::string>map)
    {
        std::map<string,string>::iterator it;
        for(it=map.begin(); it!=map.end(); ++it)
        {
            printf("\n%s=%s",it->first.c_str(),it->second.c_str());
        }
    }
    
    static void printVector(vector<std::string>vector)
    {
        std::vector<string>::iterator it;
        for(it=vector.begin(); it!=vector.end(); ++it)
        {
            printf("\n%s",it->c_str());
        }
    }
    
    static void executePost(string baseUrl,std::vector<std::string> headers, const char* postData, App42Network::SEL_App42Response pSelector)
    {
        
        App42Network::App42HttpRequest* request = new App42Network::App42HttpRequest();
        request->setUrl(baseUrl.c_str());
        request->setHttpRequestType(App42Network::App42HttpRequest::App42_POST);
        
        headers.push_back("Accept: application/json");
        headers.push_back("Content-Type: application/json");
        headers.push_back("SDKName: Cocos2d-X");
        
        //Printing Headers
        printf("Headers: ");
        Util::printVector(headers);
        printf("BaseURL=%s",baseUrl.c_str());
        
        request->setHeaders(headers);
        request->setResponseCallback(pSelector);
        
        request->setRequestData(postData, strlen(postData));
        
        App42Network::App42HttpClient::getInstance()->send(request);
        //request->release();
        return;
    }
    
    static void executePut(string baseUrl,std::vector<std::string> headers, const char* postData, App42Network::SEL_App42Response pSelector)
    {
        App42Network::App42HttpRequest* request = new App42Network::App42HttpRequest();
        request->setUrl(baseUrl.c_str());
        request->setHttpRequestType(App42Network::App42HttpRequest::App42_PUT);
        
        headers.push_back("Accept: application/json");
        headers.push_back("Content-Type: application/json");
        headers.push_back("SDKName: Cocos2d-X");
        headers.push_back("Custom-Headers: PUT");
        
        //Printing Headers
        app42Trace("Headers: ");
        Util::printVector(headers);
        app42Trace("BaseURL=%s",baseUrl.c_str());
        
        request->setHeaders(headers);
        
        request->setResponseCallback( pSelector);
        
        request->setRequestData(postData, strlen(postData));
        
        App42Network::App42HttpClient::getInstance()->send(request);
        //request->release();
        return;
    }

    static void executeGet(string baseUrl,std::vector<std::string> headers, App42Network::SEL_App42Response pSelector)
    {
        App42Network::App42HttpRequest* request = new App42Network::App42HttpRequest();
        request->setUrl(baseUrl.c_str());
        request->setHttpRequestType(App42Network::App42HttpRequest::App42_GET);
        headers.push_back("Accept: application/json");
        headers.push_back("Content-Type: application/json");
        headers.push_back("SDKName: Cocos2d-X");
        
        //Printing Headers
        app42Trace("Headers: ");
        Util::printVector(headers);
        app42Trace("BaseURL=%s",baseUrl.c_str());

        
        request->setHeaders(headers);
        request->setResponseCallback( pSelector);
        
        App42Network::App42HttpClient::getInstance()->send(request);
        //request->release();
        return;
    }
    
    static void executeDelete(string baseUrl,std::vector<std::string> headers, App42Network::SEL_App42Response pSelector)
    {
        App42Network::App42HttpRequest* request = new App42Network::App42HttpRequest();
        request->setUrl(baseUrl.c_str());
        request->setHttpRequestType(App42Network::App42HttpRequest::App42_DELETE);
        
        headers.push_back("Accept: application/json");
        headers.push_back("Content-Type: application/json");
        headers.push_back("SDKName: Cocos2d-X");
        headers.push_back("Custom-Headers:DELETE");
        
        //Printing Headers
        app42Trace("Headers: ");
        Util::printVector(headers);
        app42Trace("BaseURL=%s",baseUrl.c_str());
        
        request->setHeaders(headers);
        
        request->setResponseCallback( pSelector);
        
        App42Network::App42HttpClient::getInstance()->send(request);
        //request->release();
        return;
    }
    
    static void executeMultiPartWithFileData(string requestName,const char* fileName, unsigned char* postData,int fileDataSize,map<string,string>postParams,string baseUrl,std::vector<std::string> headers, App42Network::SEL_App42Response pSelector)
    {

        
        App42Network::App42HttpRequest* request = new App42Network::App42HttpRequest();
        request->setUrl(baseUrl.c_str());
        request->setHttpRequestType(App42Network::App42HttpRequest::App42_POSTFILEDATA);
        headers.push_back("Accept: application/json");
        headers.push_back("Content-Type: multipart/form-data");
        headers.push_back("SDKName: Cocos2d-X");
        
        //Printing Headers
        app42Trace("Headers: ");
        Util::printVector(headers);
        app42Trace("BaseURL=%s",baseUrl.c_str());
        
        
        request->setRequestName(requestName);
        request->setPostParams(postParams);
        request->setHeaders(headers);
        request->setResponseCallback( pSelector);
        request->setFileData(postData, fileDataSize);
        request->setFileName(fileName);
        App42Network::App42HttpClient::getInstance()->send(request);
        //request->release();
        return;
    }
    
    static void executeMultiPartWithFile(string requestName,string fileName, string filePath, map<string,string>postParams,string baseUrl,std::vector<std::string> headers, App42Network::SEL_App42Response pSelector)
    {
        
        App42Network::App42HttpRequest* request = new App42Network::App42HttpRequest();
        request->setUrl(baseUrl.c_str());
        request->setHttpRequestType(App42Network::App42HttpRequest::App42_POSTFILE);
        headers.push_back("Accept: application/json");
        headers.push_back("Content-Type: multipart/form-data");
        headers.push_back("SDKName: Cocos2d-X");
        
        //Printing Headers
        app42Trace("Headers: ");
        Util::printVector(headers);
        app42Trace("BaseURL=%s",baseUrl.c_str());
        
        
        request->setRequestName(requestName.c_str());
        request->setPostParams(postParams);
        request->setHeaders(headers);
        request->setResponseCallback( pSelector);
        request->setFileName(fileName);
        request->setFilePath(filePath);
        App42Network::App42HttpClient::getInstance()->send(request);
        //request->release();
        return;
    }

}

#endif
