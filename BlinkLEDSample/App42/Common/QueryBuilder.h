/**********************************************************************
 *  QueryBuilder.h
 *  App42CPPSDK
 *
 *  Created by shephertz technologies on 20/02/14.
 *  Copyright (c) 2013 ShephertzTechnology PVT LTD. All rights reserved.
 **********************************************************************/

#ifndef __App42CPPSDK__QueryBuilder__
#define __App42CPPSDK__QueryBuilder__

#include <iostream>
#include "Query.h"
#include <vector>
#include "App42Object.h"

class QueryBuilder
{
public:
    
    QueryBuilder();
    
    ~QueryBuilder();
    static Query* BuildQuery(const char* key, const char* value,const char* op);
    static Query* BuildQuery(const char* key,std::vector<std::string> value,const char* op);

    static Query* CompoundOperator(Query *q1,const char* op,Query *q2);
    static Query* SetLoggedInUser(const char* logged);

protected:
    
private:
    
};


#endif /* defined(__App42CPPSDK__QueryBuilder__) */
