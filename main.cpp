#include <iostream>
#include <cstring>
#include <cstdlib>
#include <stdlib.h>
#include <sstream>
#include <fstream>

#include <math.h>

#include "class_definitions.cpp"

using namespace std;

int main(int argc, char *argv[])
{
  string trace_file;
  char  addr_str[10];
  unsigned long addr;
  unsigned long index;
  char actual_br;
  unsigned int M2;
  unsigned long size;
  char pred_br;  
  
  Bimodal bimodal;
  
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
  fname = argv[3];


  if(strcmp(brpr_model,"bimodal") == 0)
  {
    cout<<"branch predictor type: "<<brpr_model<<endl;
    M2 = atoi(argv[2]);
  }
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

  size = pow(2,M2);
  bimodal.Bimodal_c(size);
  while(fscanf(pFile,"%s %c",&addr_str, &actual_br))
  {
    if(feof(pFile)) break;
    addr = strtoul(addr_str,NULL,16);
   // cout<<hex<<addr<<" " <<actual_br<<endl;
    addr = addr/4;
    index = addr % (unsigned long)(pow(2,M2));
    if(strcmp(brpr_model,"bimodal") == 0)
    {
    
    // cout<<" BP "<<dec<<index<<" "<<bimodal.pred_table.get_prediction_count(index);
     int prev_pred = bimodal.pred_table.get_prediction_count(index);
     if(prev_pred >= 2)
     {
       pred_br = 't';
     }
     else if(prev_pred <2)
     {
       pred_br = 'n';
     }
     // cout<<"prediction : "<<pred_br<<endl; 
      if((pred_br =='t') && (actual_br == 't'))
          { bimodal.pred_table.increment_prediction_count(index); }
      
      if((pred_br =='t') && (actual_br == 'n'))
          { bimodal.pred_table.decrement_prediction_count(index); }

      if((pred_br =='n') && (actual_br == 't'))
          { bimodal.pred_table.increment_prediction_count(index); }

      if((pred_br =='n') && (actual_br == 'n'))
          { bimodal.pred_table.decrement_prediction_count(index); }
    
    // cout<<" BU "<<dec<<index<<" "<<bimodal.pred_table.get_prediction_count(index)<<endl;
     }
  
  }
   bimodal.pred_table.print_stats(); 
  return 0;
}
