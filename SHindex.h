#ifndef SHINDEXING_H_INCLUDED
#define SHINDEXING_H_INCLUDED

class SHIndex;
#include "constants.h"
#include "data.h"
#include "myvector.h"
#include "myrandom.h"
#include <string>

using namespace std;

struct datakey
{
    int dataid;
    int key;
};

class SHIndex{
    protected:
    //for index
    int hashkeyindex[L][Alter][bucketnum];
    int hashkeylength[L][Alter][bucketnum];
    datakey temptable[datasize];

    //for query
    float queryproduct[familysize];
    unsigned int querytableresult[Alter][L];
    int queryid[datasize];
    float query[querysize][D];
    int queryresult[querysize][K];

    public:
    void index_construct(string decision_file);
    void index_write(string index_file);
    void index_load(string index_file);
    void query_load(string query_file);
    void query_execute(int);
    void result_write(string result_file);

    private:
    void pointquery(float [], int [], int,int);
};

#endif // SHINDEXING_H_INCLUDED
