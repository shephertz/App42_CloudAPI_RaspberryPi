#include <iostream>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include "App42API.h"


using namespace std;

static bool responseReceived = false;


class LinuxFile
{
private:
    int m_Handle;

public:
    LinuxFile(const char *pFile, int flags = O_RDWR)
    {
        m_Handle = open(pFile, flags);
    }

    ~LinuxFile()
    {
        if (m_Handle != -1)
            close(m_Handle);
    }

    size_t Write(const void *pBuffer, size_t size)
    {
        return write(m_Handle, pBuffer, size);
    }

    size_t Read(void *pBuffer, size_t size)
    {
        return read(m_Handle, pBuffer, size);
    }

    size_t Write(const char *pText)
    {
        return Write(pText, strlen(pText));
    }

    size_t Write(int number)
    {
        char szNum[32];
        snprintf(szNum, sizeof(szNum), "%d", number);
        return Write(szNum);
    }
};

class LinuxGPIOExporter
{
protected:
    int m_Number;

public:
    LinuxGPIOExporter(int number)
        : m_Number(number)
    {
        LinuxFile("/sys/class/gpio/export", O_WRONLY).Write(number);
    }

    ~LinuxGPIOExporter()
    {
        LinuxFile("/sys/class/gpio/unexport", 
                O_WRONLY).Write(m_Number);
    }
};

class LinuxGPIO : public LinuxGPIOExporter
{
public:
    LinuxGPIO(int number)
        : LinuxGPIOExporter(number)
    {
    }

    void SetValue(bool value)
    {
        char szFN[128];
        snprintf(szFN, sizeof(szFN), 
            "/sys/class/gpio/gpio%d/value", m_Number);
        LinuxFile(szFN).Write(value ? "1" : "0");
    }

    void SetDirection(bool isOutput)
    {
        char szFN[128];
        snprintf(szFN, sizeof(szFN), 
            "/sys/class/gpio/gpio%d/direction", m_Number);
        LinuxFile(szFN).Write(isOutput ? "out" : "in");
    }
};


class SampleClass : public App42CallBack
{
public:
    void onPushRequestCompleted( void *response);
    void onEventRequestCompleted( void *response);
    void onStorageRequestCompleted( void *response);

};

/**
 * Callbacks
 */
void SampleClass::onPushRequestCompleted(void *response)
{
    App42PushNotificationResponse *pushResponse = (App42PushNotificationResponse*)response;
    printf("\ncode=%d",pushResponse->getCode());
    printf("\nResponse Body=%s",pushResponse->getBody().c_str());
    
    if (pushResponse->isSuccess)
    {
        for(std::vector<App42PushNotification>::iterator it = pushResponse->notifications.begin(); it != pushResponse->notifications.end(); ++it)
        {
            printf("\n UserName=%s",it->userName.c_str());
            printf("\n Message=%s\n",it->message.c_str());
            printf("\n Type=%s\n",it->type.c_str());
            printf("\n DeviceToken=%s\n",it->deviceToken.c_str());
            printf("\n Expiry=%s\n",it->expiry.c_str());
            
            for(std::vector<App42Channel>::iterator channel = it->channelArray.begin(); channel != it->channelArray.end(); ++channel)
            {
                printf("\n Channel Name=%s",channel->channelName.c_str());
                printf("\n Description=%s\n",channel->description.c_str());
            }
        }
    }
    else
    {
        printf("\nerrordetails:%s",pushResponse->errorDetails.c_str());
        printf("\nerrorMessage:%s",pushResponse->errorMessage.c_str());
        printf("\nappErrorCode:%d",pushResponse->appErrorCode);
        printf("\nhttpErrorCode:%d",pushResponse->httpErrorCode);
    }
    responseReceived = true;
}


void SampleClass::onStorageRequestCompleted( void *response)
{
    App42StorageResponse *storageResponse = (App42StorageResponse*)response;
    printf("\ncode=%d",storageResponse->getCode());
    printf("\nResponse Body=%s",storageResponse->getBody().c_str());
    
    if (storageResponse->isSuccess)
    {
        printf("\nTotalRecords=%d",storageResponse->getTotalRecords());
        for(std::vector<App42Storage>::iterator it = storageResponse->storages.begin(); it != storageResponse->storages.end(); ++it)
        {
            printf("\n DbName=%s",it->dbName.c_str());
            printf("\n CollectionName=%s",it->collectionName.c_str());
            printf("\n RecordCount=%lf\n",it->recordCount);
        }
    }
    else
    {
        printf("\nerrordetails:%s",storageResponse->errorDetails.c_str());
        printf("\nerrorMessage:%s",storageResponse->errorMessage.c_str());
        printf("\nappErrorCode:%d",storageResponse->appErrorCode);
        printf("\nhttpErrorCode:%d",storageResponse->httpErrorCode);
    }
    responseReceived = true;
}


void SampleClass::onEventRequestCompleted( void *response)
{
    App42Response *app42Response = (App42Response*)response;
    printf("\ncode=%d...=%d",app42Response->getCode(),app42Response->isSuccess);
    printf("\nResponse Body=%s",app42Response->getBody().c_str());
    
    
    if (app42Response->isSuccess)
    {
        printf("\nTotalRecords=%d",app42Response->getTotalRecords());
    }
    else
    {
        printf("\nerrordetails:%s",app42Response->errorDetails.c_str());
        printf("\nerrorMessage:%s",app42Response->errorMessage.c_str());
        printf("\nappErrorCode:%d",app42Response->appErrorCode);
        printf("\nhttpErrorCode:%d",app42Response->httpErrorCode); 
    }
    responseReceived = true;
}



int main ()
{
    cout<<"Main Called"<<endl;
    SampleClass *sampleClassInstance = new SampleClass();

    App42API::Initialize("67359321652c10b15fd5f659d096a2051745aa4f339b936d6ce5dccb165de863","2d44878dd428046f23bd4b3807ce83e982db9900f4c5a8305255da4764c76610");
    
    /**
     * Uncomment to test Storage Service
     */
    /*
    StorageService *storageService = App42API::BuildStorageService();
    const char* dbName = "jsonDocument2";
    const char* collectionName = "Your_Collection_Name";
    const char* jsonDoc ="{\"name\":\"Nick\",\"age\":30,\"phone\":\"xxx-xxx-xxx\"}";
    storageService->InsertJsonDocument(dbName, collectionName,jsonDoc, app42callback(SampleClass::onStorageRequestCompleted, sampleClassInstance));
     */
    
    
    /**
     * Uncomment to test Event Service
     */
    /*App42API::setLoggedInUser("Shephertz");
    EventService *eventService = App42API::BuildEventService();
    App42Object *app42Object = new App42Object();
    app42Object->setObject("Name","RajeevRShephertz");
    //eventService->SetLoggedInUserProperties(app42Object,app42callback(SampleClass::onEventRequestCompleted, sampleClassInstance));
    eventService->TrackEvent("LEDOnEvent", app42Object,app42callback(SampleClass::onEventRequestCompleted, sampleClassInstance));*/
    
    /**
     * ------- UNCOMMENT THE WHILE LOOP TO TEST ANY OF THE ABOVE SERVICES ---------
     * Not required for LED Blink Test
     */
    /* while (!responseReceived)
    {
        printf("Served the purpose");
    }
    */
    
    
    /**
     * Run Blink LED
     */
    int onCounter = 1;
    LinuxGPIO gpio27(27);
    gpio27.SetDirection(true);
    bool on = true;
    for (;;)
    {
        printf("Switching %s the LED...\n", on ? "on" : "off");
        gpio27.SetValue(on);
        
        if(onCounter%10==0)
        {
			App42API::setLoggedInUser("Shephertz");
			EventService *eventService = App42API::BuildEventService();
			App42Object *app42Object = new App42Object();
			app42Object->setObject("Name","Shephertz");
			eventService->TrackEvent("LEDOnEvent", app42Object,app42callback(SampleClass::onEventRequestCompleted, sampleClassInstance));
		}
        onCounter++;
        on = !on;
        sleep(1);
    }
    
    
    printf("\nServed the purpose\n");
    return 0;
}


