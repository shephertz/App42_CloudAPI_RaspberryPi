//
//  App42ACL.cpp
//  App42Cocos2dX3.0Sample
//
//  Created by Shephertz Technologies Pvt Ltd on 24/07/14.
//
//

#include "ACL.h"


App42ACL::App42ACL(const char* _userName, Permission _permission)
{
    userName = _userName;
    permission = getPermission(_permission);
}

App42ACL::App42ACL()
{
    userName = "";
    permission = "";
}


App42ACL::~App42ACL()
{
    
}

void App42ACL::setUserName(string _userName)
{
    userName = _userName;
}
void App42ACL::setPermission(Permission _permission)
{
    permission = getPermission(_permission);
}

void App42ACL::setPermission(string _permission)
{
    permission = _permission;
}

string App42ACL::getUserName()
{
    return userName;
}
string App42ACL::getPermission()
{
    return permission;
}

string App42ACL::toString()
{
    string App42ACLString = "User : ";
    App42ACLString.append(userName);
    App42ACLString.append(" Permission : ");
    App42ACLString.append(permission);
    return App42ACLString;
}

bool App42ACL::equals(App42ACL *App42ACLObject)
{
    if (App42ACLObject == NULL)
    {
        return false;
    }
    else
    {
        if (App42ACLObject->userName.length() && App42ACLObject->permission.length() && (App42ACLObject->userName.compare(userName) ==0)&& (App42ACLObject->permission.compare(permission)==0))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}
