#include "SHindex.h"
#include <iostream>
#include <cmath>
#include <algorithm>

using namespace std;

bool datakeycompare(datakey a,datakey b)
{
      if(a.key < b.key) return true;
      if(a.key == b.key)
      {
          if(a.dataid < b.dataid)return true;
      }
      return false;
}

void SHIndex::index_construct(string decision_file)
{
     shg.init();
     if(!shg.decisionavailable)io.diskread_int(decision_file.c_str(), shg.decision, datasize);
     for(int k = 0; k < datasize; k++)
     {
            if(k%20000 == 0) cout<<"current hashing data "<<k<<endl;
            shg.tableindex(shg.dataproduct[k],shg.decision[k],shg.datahashresult[k]);
     }
     for(int j = 0; j < L; j++)
     {
             int hashkey;
             for(int i = 0; i < datasize; i++)
             {
                     temptable[i].dataid = i;
                     hashkey = shg.datahashresult[i][j]%bucketnum;
                     temptable[i].key = shg.decision[i]*bucketnum + hashkey;
             }
             sort(temptable,temptable+datasize,datakeycompare);
             int q = 0;
             hashkeyindex[j][0][0] = 0;
             hashkeylength[j][0][0] = 0;
             for(int i = 0; i < datasize; i++)
             {
                     while(q != temptable[i].key)
                     {
                         q++;
                         hashkeyindex[j][q/bucketnum][q%bucketnum] = i;
                         hashkeylength[j][q/bucketnum][q%bucketnum] = 0;
                     }
                     hashkeylength[j][q/bucketnum][q%bucketnum]++;
                     shg.datahashtable[j][i] = temptable[i].dataid;
             }
             //for test
             int nullsum = 0;
             int normalsum = 0;
             for(int i = 0; i < Alter * bucketnum; i++)
             {
                 if(hashkeylength[j][i/bucketnum][i%bucketnum] == 0) nullsum++;
                 else normalsum++;
             }
             cout<<"table "<<j<<" construted"<<endl;
             cout<<nullsum<<" "<<normalsum<<endl;
     }
	 //for test
	 double sumchecknum = 0;
	 double optimized = 0;
	 double avechecknum;
	 for(int i = 0; i < L; i++)
	 {
		 for(int k = 0; k < Alter; k++)
		 {
            for(int j = 0; j < bucketnum; j++)
            {
                sumchecknum += hashkeylength[i][k][j] * hashkeylength[i][k][j] / (double) datasize;
                optimized += hashkeylength[i][k][j] / (double) bucketnum;
            }
		 }
	 }
	 avechecknum = sumchecknum / L;
	 cout<<"bucket balance index is "<<avechecknum<<endl;
	 cout<<"optimized bucket balance is "<<optimized / L<<endl;
}


void SHIndex::index_write(string index_file)
{
    FILE *fp;
	fp = fopen(index_file.c_str(),"wb");
	if(fp == NULL)
	{
	    cout << "Cannot open file!" << endl;;
		exit(1);
	}
	fwrite(shg.familyvector[0], sizeof(float), familysize*(D+1), fp);
	fwrite(shg.hashtableindex[0], sizeof(int), L*M, fp);
	fwrite(shg.datahashresult[0], sizeof(unsigned int), datasize*L, fp);
	fwrite(shg.datahashtable[0], sizeof(int), L*datasize, fp);
	fwrite(hashkeyindex[0][0], sizeof(int), L * Alter * bucketnum, fp);
	fwrite(hashkeylength[0][0], sizeof(int), L * Alter * bucketnum, fp);
	fclose(fp);
}

void SHIndex::index_load(string index_file)
{
    FILE *fp;
	fp = fopen(index_file.c_str(),"rb");
	if(fp == NULL)
	{
	    cout << "Cannot open file!" << endl;;
		exit(1);
	}
	fread(shg.familyvector[0], sizeof(float), familysize*(D+1), fp);
	fread(shg.hashtableindex[0], sizeof(int), L*M, fp);
	fread(shg.datahashresult[0], sizeof(unsigned int), datasize*L, fp);
	fread(shg.datahashtable[0], sizeof(int), L*datasize, fp);
	fread(hashkeyindex[0][0], sizeof(int), L * Alter * bucketnum, fp);
	fread(hashkeylength[0][0], sizeof(int), L * Alter * bucketnum, fp);
	fclose(fp);
}

void SHIndex::query_load(string query_file)
{
     io.diskread_float(query_file.c_str(), query[0], querysize*D);
     for(int i = 0; i < datasize; i++)
     {
             queryid[i] = -1;
     }
     st.sumcheck = 0;
}

void SHIndex::query_execute(int Lused)
{
     for(int i = 0; i < querysize; i++)
     {
             pointquery(query[i],queryresult[i],i,Lused);
             //cout<<"doing query id: "<<i<<" "<<st.sumcheck<<endl;
     }
}

void SHIndex::result_write(string result_file)
{
     io.diskwrite_int(result_file.c_str(), queryresult[0], querysize*K);
}

void SHIndex::pointquery(float querypoint[], int result[], int id,int Lused)
{
     float queryproduct[familysize];
     for(int i = 0; i < familysize; i++)queryproduct[i] = MyVector::dotproduct(D,querypoint,shg.familyvector[i]);
     for(int i = 0; i < Alter; i++)shg.tableindex(queryproduct,i,querytableresult[i]);
     knn.init();
	 int hashkey,bucketindex,bucketlength,tocheck;
	 for(int n = 0; n < Alter; n++)
	 {
              if(knn.sqrtbound < ETRatio*shg.R[n])break;
              for(int i = 0; i < Lused; i++)
              {
                    hashkey = querytableresult[n][i]%bucketnum;
                    bucketindex = hashkeyindex[i][n][hashkey];
                    bucketlength = hashkeylength[i][n][hashkey];
                    for(int j = 0; j < bucketlength; j++)
                    {
                        tocheck = shg.datahashtable[i][bucketindex+j];
                        if (shg.datahashresult[tocheck][i] != querytableresult[n][i])continue;
                        if (queryid[tocheck] == id) continue;
                        queryid[tocheck] = id;
                        knn.addvertex(data, tocheck, querypoint);
                        st.sumcheck++;
                    }
              }
     }
     for(int i = 0; i < K; i++)result[i] = knn.knnlist[i];
     //for(int n = 0; n < Alter; n++)cout<<n<<" number checked: "<<sumcheck[n]<<endl;
}
