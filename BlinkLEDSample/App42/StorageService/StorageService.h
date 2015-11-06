//
//  StorageService.h
//  App42CPPSDK
//
//  Created by Rajeev Ranjan on 21/10/13.
//
//

#ifndef __App42CPPSDK__StorageService__
#define __App42CPPSDK__StorageService__

#include <iostream>
#include "App42Service.h"



using namespace std;

class StorageService : public App42Service
{
    
private:
    StorageService();
	static StorageService* _instance;
public:
    
	/**
     * This is a constructor that takes
     *
     * @param apiKey
     * @param secretKey
     * @param server
     *
     */
	
    static StorageService* Initialize(string apikey, string secretkey);
    
	static void Terminate();
    
	/**
     * Builds the instance of StorageService.
     *
     * @return StorageService - StorageService Object
     */
	static StorageService* getInstance();
    
    void InsertJsonDocument(const char* dbName, const char* collectionName, const char* json, const app42CallBack& pSelector);
    
    void InsertJsonDocument(const char* dbName, const char* collectionName, App42Object *app42Object, const app42CallBack& pSelector);

    
	void FindAllCollections(const char* dbName, const app42CallBack& pSelector);
    
	void FindAllDocuments(const char* dbName, const char* collectionName, const app42CallBack& pSelector);
    
    void FindAllDocuments(const char* dbName, const char* collectionName,int max, int offset, const app42CallBack& pSelector);
    
    void FindAllDocumentsCount(const char* dbName, const char* collectionName, const app42CallBack& pSelector);
    
	void FindDocumentById(const char* dbName, const char* collectionName, const char* docId, const app42CallBack& pSelector);
    
    //Deprecated---Use "FindDocumentsByQuery"
    void FindDocumentByQuery(const char* dbName, const char* collectionName, Query *query, const app42CallBack& pSelector);
    
    void FindDocumentsByQuery(const char* dbName, const char* collectionName, Query *query, const app42CallBack& pSelector);
    
    void FindDocumentsByQueryWithPaging(const char* dbName, const char* collectionName, Query *query, int max, int offset, const app42CallBack& pSelector);
    
    void FindDocsWithQueryPagingOrderBy(const char* dbName, const char* collectionName, Query *query, int max, int offset,const char* orderByKey,const char* orderByType, const app42CallBack& pSelector);
    
	void FindDocumentByKeyValue(const char* dbName, const char* collectionName, const char* key,const char* value, const app42CallBack& pSelector);
    
    void UpdateDocumentByDocId(const char* dbName, const char* collectionName, const char* docId,const char* newJsonDoc, const app42CallBack& pSelector);
    
    void UpdateDocumentByDocId(const char* dbName, const char* collectionName, const char* docId, App42Object *app42Object, const app42CallBack& pSelector);
    
    void UpdateDocumentByKeyValue(const char* dbName, const char* collectionName, const char* key,const char* value,const char* json, const app42CallBack& pSelector);
    
    void UpdateDocumentByKeyValue(const char* dbName, const char* collectionName, const char* key,const char* value,App42Object *app42Object, const app42CallBack& pSelector);
    
    void SaveOrUpdateDocumentByKeyValue(const char* dbName, const char* collectionName, const char* key,const char* value,const char* json, const app42CallBack& pSelector);
    
    void SaveOrUpdateDocumentByKeyValue(const char* dbName, const char* collectionName, const char* key,const char* value,App42Object *app42Object, const app42CallBack& pSelector);

    void DeleteDocumentsById(const char* dbName, const char* collectionName, const char* docId, const app42CallBack& pSelector);
    
    void DeleteDocumentsByKeyValue(const char* dbName, const char* collectionName, const char* key, const char* value, const app42CallBack& pSelector);

    void DeleteAllDocuments(const char* dbName, const char* collectionName, const app42CallBack& pSelector);
    
    void AddOrUpdateKeys(const char* dbName, const char* collectionName, const char* docId,const char* json, const app42CallBack& pSelector);
    
    void AddOrUpdateKeys(const char* dbName, const char* collectionName, const char* docId,App42Object *app42Object, const app42CallBack& pSelector);
    
    void UpdateDocumentByQuery(const char* dbName, const char* collectionName, Query *query, const char* json, const app42CallBack& pSelector);
    
    void UpdateDocumentByQuery(const char* dbName, const char* collectionName, Query *query, App42Object *app42Object, const app42CallBack& pSelector);
    
    /**
	 *
	 * @param dbName
	 * @param collectionName
	 * @param docId
	 * @param App42ACLList
	 * @return
	 * @throws App42Exception
	 */
	void GrantAccessOnDoc(const char* dbName, const char* collectionName,
                          const char* docId, vector<App42ACL> App42ACLList, const app42CallBack& pSelector);
    
    /**
	 *
	 * @param dbName
	 * @param collectionName
	 * @param docId
	 * @param App42ACLList
	 * @return
	 * @throws App42Exception
	 */
	void RevokeAccessOnDoc(const char* dbName, const char* collectionName,
                           const char* docId, vector<App42ACL> App42ACLList, const app42CallBack& pSelector);
};

#endif /* defined(__App42CPPSDK__StorageService__) */
