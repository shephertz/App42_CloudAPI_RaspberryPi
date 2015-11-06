//
//  JSONDocument.h
//  App42CPPSDK
//
//  Created by Rajeev Ranjan on 25/02/14.
//
//

#ifndef __App42CPPSDK__JSONDocument__
#define __App42CPPSDK__JSONDocument__

#include <iostream>
#include "ACL.h"
#include <vector>
using namespace std;
 

class JSONDocument
{
public:
/**
 * Getters
 */
    string getJsonDoc(){return jsonDoc;};
    string getDocId(){return docId;};
    string getEvent(){return event;};
    string getCreatedAt()
    {
        return createdAt;
    };
    string getUpdatedAt(){return updatedAt;};
    string getOwner(){return owner;};
    vector<App42ACL> getApp42ACLList(){return App42ACLList;};
    
/**
 * Setters
 */
    void setJsonDoc(string _jsonDoc){jsonDoc=_jsonDoc;};
    void setDocId(string _docId){docId=_docId;};
    void setEvent(string _event){event=_event;};
    void setCreatedAt(string _createdAt){createdAt=_createdAt;};
    void setUpdatedAt(string _updatedAt){updatedAt=_updatedAt;};
    void setOwner(string _owner){owner=_owner;};
    

    void pushToApp42ACLList(App42ACL *App42ACL){ App42ACLList.push_back(*App42ACL);};
    string toString();
    
protected:
    string jsonDoc;
    string createdAt;
    string event;
    string updatedAt;
    string docId;
    vector<App42ACL> App42ACLList;
    
private:
    string owner;
};
#endif /* defined(__App42CPPSDK__JSONDocument__) */
