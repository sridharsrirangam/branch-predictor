#include <iostream>
#include <cstring>
#include <cstdlib>
#include <stdlib.h>
#include <sstream>
#include <fstream>

using namespace std;

int main(int argc, char *argv[])
{
  string trace_file;
  char  addr_str[10];
  unsigned long addr;
  char actual_br;


  if(argc != 2)
  {
    cout<<"invalid arguments"<<endl;
  }
  //read arguments
  //
  char * brpr_model = (char *)malloc(20); 
  brpr_model = argv[1];
  
  FILE *pFile;
  char *fname = (char *)malloc(30);
  fname = argv[2];


  if(strcmp(brpr_model,"bimodal") == 0)
  cout<<"branch predictor type: "<<brpr_model<<endl;
  
  if(strcmp(brpr_model,"gshare") == 0)
  cout<<"branch predictor type: "<<brpr_model<<endl;
  
  if(strcmp(brpr_model,"hybrid") == 0)
  cout<<"branch predictor type: "<<brpr_model<<endl;
  
  cout<<"trace file: "<<fname<<endl;

  pFile = fopen (fname,"r");
  if(pFile == 0)
  {
    cout<<"trace file problem"<<endl;
    exit(0);
  }
  while(fscanf(pFile,"%s %c",&addr_str, &actual_br))
  {
    if(feof(pFile)) break;
    addr = strtoul(addr_str,NULL,16);
    cout<<"addr is: "<<addr<<" actual taken: "<<actual_br<<endl;
  }
    
  return 0;

}
