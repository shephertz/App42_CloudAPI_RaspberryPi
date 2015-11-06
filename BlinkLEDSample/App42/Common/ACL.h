//
//  ACL.h
//  App42Cocos2dX3.0Sample
//
//  Created by Shephertz Technologies Pvt Ltd on 24/07/14.
//
//

#ifndef __App42Cocos2dX3_0Sample__App42ACL__
#define __App42Cocos2dX3_0Sample__App42ACL__

#include <iostream>
#include "cJSON.h"

using namespace std;
    
typedef enum Permission
{
    R,
    W
}Permission;

class App42ACL
{
public:
    App42ACL(const char* _userName, Permission _permission);
    App42ACL();
    ~App42ACL();
    
    /**
     * Setters
     */
    void setUserName(string _userName);
    void setPermission(Permission _permission);
    void setPermission(string _permission);
    /**
     * Getters
     */
    string getUserName();
    string getPermission();
    
    string toString();
    
    bool equals(App42ACL *App42ACLObject);

private:
    string userName;
    string permission;
    
    inline const char *getPermission(Permission _permission)
    {
        const char *strings[] = {"R","W"};
        
        return strings[_permission];
    }
};

#endif /* defined(__App42Cocos2dX3_0Sample__App42ACL__) */
