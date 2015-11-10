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
  unsigned int gshare_n;
  unsigned long ghb = 0;

  Bimodal bimodal;
  Gshare gshare; 
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


  if(strcmp(brpr_model,"bimodal") == 0)
  {
    cout<<"branch predictor type: "<<brpr_model<<endl;
    M2 = atoi(argv[2]);
    fname = argv[3];
  }
  if(strcmp(brpr_model,"gshare") == 0)
  {
    cout<<"branch predictor type: "<<brpr_model<<endl;
    M2 = atoi(argv[2]);
    gshare_n = atoi(argv[3]);
    fname = argv[4];
  } 
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
  gshare.Gshare_c(size);
  while(fscanf(pFile,"%s %c",&addr_str, &actual_br))
  {
    if(feof(pFile)) break;
    addr = strtoul(addr_str,NULL,16);
   // cout<<hex<<addr<<" " <<actual_br<<endl;
    addr = addr/4; //ignoring last 2 bits of PC
   
    // BIMODAL beginning ******************************************************************************

    if(strcmp(brpr_model,"bimodal") == 0)
    {
    
    // cout<<" BP "<<dec<<index<<" "<<bimodal.pred_table.get_prediction_count(index);
     index = addr % (unsigned long)(pow(2,M2));
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


    // Gshare beggining ***********************************************************************************

    if(strcmp(brpr_model,"gshare") == 0)
    { 
      unsigned int size_n = (unsigned int)(pow(2,gshare_n));
      unsigned int size_m = (unsigned int)(pow(2,M2));
      unsigned int size_m_n = (unsigned int)(pow(2, M2 - gshare_n));

      ghb = ghb % size_n;
      unsigned long addr_m_bits = addr % size_m;
      unsigned long ghb_xor = ghb << (M2 - gshare_n);
      unsigned int index = addr_m_bits ^ ghb_xor;
       
       cout<<" BP "<<dec<<index<<" "<<gshare.pred_table.get_prediction_count(index)<<endl;




    }
  
  }
    if(strcmp(brpr_model,"bimodal") == 0)  bimodal.pred_table.print_stats(); 
    if(strcmp(brpr_model,"bimodal") == 0)
  return 0;
}
