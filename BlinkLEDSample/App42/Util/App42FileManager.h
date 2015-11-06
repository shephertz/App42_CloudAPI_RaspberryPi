//
//  App42FileManager.h
//  App42Cocos2dX3.0Sample
//
//  Created by Rajeev Ranjan on 06/08/15.
//
//

#ifndef __App42Cocos2dX3_0Sample__App42FileManager__
#define __App42Cocos2dX3_0Sample__App42FileManager__

#include <iostream>
#include <stdio.h>

using namespace std;

namespace Util
{
    static string createInstallationId()
    {
        char strUuid[100];
        srand((unsigned int)time(NULL));
        
        sprintf(strUuid, "%x%x-%x-%x-%x-%x%x%x",
                rand(), rand(),                 // Generates a 64-bit Hex number
                rand(),                         // Generates a 32-bit Hex number
                ((rand() & 0x0fff) | 0x4000),   // Generates a 32-bit Hex number of the form 4xxx (4 indicates the UUID version)
                rand() % 0x3fff + 0x8000,       // Generates a 32-bit Hex number in the range [0x8000, 0xbfff]
                rand(), rand(), rand());        // Generates a 96-bit Hex number
        
        printf("GUUID = %s",strUuid);
        return strUuid;
    }
    
    static string getInstallationId()
    {
        FILE *fileManager;
        unsigned char file_data[100];
        const char *fileName = "App42Config.txt";
        string installationId = "";
        fileManager = fopen(fileName, "rb");
        if(fileManager)
        {
            fread(&file_data[0],sizeof(unsigned char),100,fileManager);
            
            fclose(fileManager);
        }
        else
        {
            cout<<"File Not Found"<<endl;
            //Create a new file
            fileManager = fopen(fileName, "wb");
            if(fileManager)
            {
                cout<<"Writing content"<<endl;
                string installationId  = createInstallationId();
                fwrite(installationId.c_str(),sizeof(unsigned char),100,fileManager);
                printf("%s",file_data);
                fclose(fileManager);
            }
        }
        
        stringstream s;
        s << file_data;
        installationId = s.str();
        return installationId;
    }
}

#endif /* defined(__App42Cocos2dX3_0Sample__App42Object__) */
