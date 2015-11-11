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
  unsigned int gshare_n = 0;
  unsigned long ghb = 0;

  unsigned long total_reads = 0;
  unsigned long mispredicts = 0;
  Bimodal bimodal;
  Gshare gshare;
  Bimodal hybrid_bimodal;
  Gshare hybrid_gshare;

  // variables for hybrid
  unsigned int hybrid_M1 = 0 , hybrid_M2 = 0, hybrid_k = 0,hybrid_n = 0;
  BTB hybrid_counter;
  
  if(argc != 2)
  {
    //cout<<"invalid arguments"<<endl;
  }
  //read arguments
  //
  char * brpr_model = (char *)malloc(20); 
  brpr_model = argv[1];
  
  FILE *pFile;
  char *fname = (char *)malloc(30);


  if(strcmp(brpr_model,"bimodal") == 0)
  {
   // cout<<"branch predictor type: "<<brpr_model<<endl;
    M2 = atoi(argv[2]);
    fname = argv[3];
  }
  if(strcmp(brpr_model,"gshare") == 0)
  {
   // cout<<"branch predictor type: "<<brpr_model<<endl;
    M2 = atoi(argv[2]);
    gshare_n = atoi(argv[3]);
    fname = argv[4];
  } 
  if(strcmp(brpr_model,"hybrid") == 0)
  {
    hybrid_k = atoi(argv[2]);
    hybrid_M1 = atoi(argv[3]);
    hybrid_n = atoi(argv[4]);
    hybrid_M2 = atoi(argv[5]);
    fname = argv[6];

  }
  //cout<<"branch predictor type: "<<brpr_model<<endl;
  
  unsigned int size_M1 = pow(2,hybrid_M1);
  unsigned int size_M2 = pow(2,hybrid_M2);
  unsigned int size_k = pow(2,hybrid_k);
  
  cout<<"COMMAND"<<endl;
  cout<<"./sim "<<brpr_model<<" "<<M2;
  if(gshare_n != 0) cout<<" "<<gshare_n<<" ";
  cout<<" "<<fname<<endl; 
 // cout<<"trace file: "<<fname<<endl;
  
  pFile = fopen (fname,"r");
  if(pFile == 0)
  {
    cout<<"trace file problem"<<endl;
    exit(0);
  }

  size = pow(2,M2);
  
  bimodal.Bimodal_c(size);
  gshare.Gshare_c(size);
  hybrid_counter.BTB_c(size_k);
  hybrid_bimodal.Bimodal_c(size_M2);
  hybrid_gshare.Gshare_c(size_M1);
  hybrid_counter.set_count_to_1();
 
  while(fscanf(pFile,"%s %c",&addr_str, &actual_br))
  {
    if(feof(pFile)) break;
    total_reads++;
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
          { 
            bimodal.pred_table.increment_prediction_count(index); 
          }
          
      
      if((pred_br =='t') && (actual_br == 'n'))
          {
            bimodal.pred_table.decrement_prediction_count(index); 
            mispredicts++;
          }
          

      if((pred_br =='n') && (actual_br == 't'))
          {
            bimodal.pred_table.increment_prediction_count(index); 
            mispredicts++;
          }
          

      if((pred_br =='n') && (actual_br == 'n'))
          {
            bimodal.pred_table.decrement_prediction_count(index);
          }
    
    // cout<<" BU "<<dec<<index<<" "<<bimodal.pred_table.get_prediction_count(index)<<endl;
     }


    // Gshare beggining ***********************************************************************************

    if(strcmp(brpr_model,"gshare") == 0)
    { 
      unsigned int size_n = (unsigned int)(pow(2,gshare_n));
      unsigned int size_m = (unsigned int)(pow(2,M2));
      unsigned int size_m_n = (unsigned int)(pow(2, M2 - gshare_n));

      ghb = ghb % size_n;
     // cout<<"ghb after modulo n " <<ghb<<endl;

      unsigned long addr_m_bits = addr % size_m;
      unsigned long ghb_xor = ghb << (M2 - gshare_n);
     // cout<<"ghb after shift "<<dec<<ghb_xor<<" m "<<M2<<" n "<<gshare_n<<endl;
      unsigned int index = addr_m_bits ^ ghb_xor;
      int prev_pred = gshare.pred_table.get_prediction_count(index);
      if(prev_pred >= 2)
      {
         pred_br = 't';
      }  
       else if(prev_pred <2)
      {
         pred_br = 'n';
      }
       
      // cout<<" BP "<<dec<<index<<" "<<gshare.pred_table.get_prediction_count(index)<<endl;
      if((pred_br =='t') && (actual_br == 't'))
          { gshare.pred_table.increment_prediction_count(index);
         //   cout<<"ghb: "<<ghb<<endl;
            ghb = ghb >> 1;
            ghb = ghb + (unsigned int)(pow(2,gshare_n-1));
           // cout<<"ghb after updating : " <<ghb<<endl;
          }
      
      if((pred_br =='t') && (actual_br == 'n'))
          {
           // cout<<"ghb: "<<ghb<<endl;
            gshare.pred_table.decrement_prediction_count(index); 
            ghb = ghb >> 1;
            mispredicts++;
           // cout<<"ghb after updating : " <<ghb<<endl;
          }

      if((pred_br =='n') && (actual_br == 't'))
          {
           // cout<<"ghb: "<<ghb<<endl;
            gshare.pred_table.increment_prediction_count(index);
            ghb = ghb >> 1;
            ghb = ghb + (unsigned int)(pow(2,gshare_n-1));
            mispredicts++;
           // cout<<"ghb after updating : " <<ghb<<endl;
          }
      
      if((pred_br =='n') && (actual_br == 'n'))
          {
           // cout<<"ghb: "<<ghb<<endl;
            gshare.pred_table.decrement_prediction_count(index);
            ghb = ghb >> 1;
           // cout<<"ghb after updating : " <<ghb<<endl;
          }




    }

    // hybrid  beggining ***********************************************************************************
    if(strcmp(brpr_model,"hybrid") == 0)
    {
     // cout<<"inside hybrid"<<endl;
      unsigned long hybrid_counter_index = addr % (unsigned long)(pow(2,hybrid_k));
      char pred_bimodal, pred_gshare, pred_chooser;
      int gshare_correct = 0, bimodal_correct = 0;
      
      unsigned long bimodal_index = addr % (unsigned long)(pow(2,hybrid_M2));
     
      unsigned int size_n = (unsigned int)(pow(2,hybrid_n));
      unsigned int size_m = (unsigned int)(pow(2,hybrid_M1));
      unsigned int size_m_n = (unsigned int)(pow(2, hybrid_M1 - hybrid_n));
      ghb = ghb % size_n;
     // cout<<"ghb after modulo n " <<ghb<<endl;

      unsigned long addr_m_bits = addr % size_m;
      unsigned long ghb_xor = ghb << (hybrid_M1 - hybrid_n);
     // cout<<"ghb after shift "<<dec<<ghb_xor<<" m "<<M2<<" n "<<gshare_n<<endl;
      unsigned int gshare_index = addr_m_bits ^ ghb_xor;
       
      int prev_pred_bimodal = hybrid_bimodal.pred_table.get_prediction_count(bimodal_index);
      if(prev_pred_bimodal >= 2)
      {
       pred_bimodal = 't';
      }
      else if(prev_pred_bimodal <2)
      {
       pred_bimodal = 'n';
      }
      int prev_pred_gshare = hybrid_gshare.pred_table.get_prediction_count(gshare_index);
      if(prev_pred_gshare >= 2)
      {
         pred_gshare = 't';
       }
       else if(prev_pred_gshare <2)
      {
         pred_gshare = 'n';
      }
      // cout<<"prediction : "<<pred_br<<endl; 
     
       
     int prev_pred_chooser = hybrid_counter.get_prediction_count(hybrid_counter_index);
      if(((pred_gshare == 't')&&(actual_br == 't')) || (pred_gshare == 'n')&&(actual_br == 'n'))
      {
        gshare_correct = 1;
      }
      if(((pred_bimodal == 't')&&(actual_br == 't')) || (pred_bimodal == 'n')&&(actual_br == 'n'))
      {
        bimodal_correct = 1;
      }
     // cout<<"GP: "<<dec<<gshare_index<<" "<<prev_pred_gshare<<endl;
     // cout<<"BP: "<<bimodal_index<<" "<<prev_pred_bimodal<<endl;
     // cout<<"CP: "<<hybrid_counter_index<<" "<<prev_pred_chooser<<endl;

     if(prev_pred_chooser < 2) //choose bimodal 
     {
       if((pred_bimodal =='t') && (actual_br == 't'))
          { 
           hybrid_bimodal.pred_table.increment_prediction_count(bimodal_index); 
          }
          
      
      if((pred_bimodal =='t') && (actual_br == 'n'))
          {
            hybrid_bimodal.pred_table.decrement_prediction_count(bimodal_index); 
           mispredicts++;
          }
          

      if((pred_bimodal =='n') && (actual_br == 't'))
          {
           hybrid_bimodal.pred_table.increment_prediction_count(bimodal_index); 
           mispredicts++;
          }
          

      if((pred_bimodal =='n') && (actual_br == 'n'))
          {
           hybrid_bimodal.pred_table.decrement_prediction_count(bimodal_index);
          } 
    // cout<<" BU "<<dec<<bimodal_index<<" "<<hybrid_bimodal.pred_table.get_prediction_count(bimodal_index)<<endl;
     } 
     if(prev_pred_chooser >= 2) //choose gshare
     {
      if((pred_gshare =='t') && (actual_br == 't'))
          { hybrid_gshare.pred_table.increment_prediction_count(gshare_index);
         //   cout<<"ghb: "<<ghb<<endl;
          //  ghb = ghb >> 1;
          //  ghb = ghb + (unsigned int)(pow(2,gshare_n-1));
           // cout<<"ghb after updating : " <<ghb<<endl;
          }
      
      if((pred_gshare =='t') && (actual_br == 'n'))
          {
           // cout<<"ghb: "<<ghb<<endl;
            hybrid_gshare.pred_table.decrement_prediction_count(gshare_index); 
           // ghb = ghb >> 1;
            mispredicts++;
           // cout<<"ghb after updating : " <<ghb<<endl;
          }

      if((pred_gshare =='n') && (actual_br == 't'))
          {
           // cout<<"ghb: "<<ghb<<endl;
            hybrid_gshare.pred_table.increment_prediction_count(gshare_index);
           // ghb = ghb >> 1;
           // ghb = ghb + (unsigned int)(pow(2,gshare_n-1));
            mispredicts++;
           // cout<<"ghb after updating : " <<ghb<<endl;
          }
      
      if((pred_gshare =='n') && (actual_br == 'n'))
          {
           // cout<<"ghb: "<<ghb<<endl;
            hybrid_gshare.pred_table.decrement_prediction_count(gshare_index);
           // ghb = ghb >> 1;
           // cout<<"ghb after updating : " <<ghb<<endl;
          }
      }

      if((pred_gshare =='t') && (actual_br == 't'))
        {
        //   cout<<"ghb: "<<ghb<<endl;
            ghb = ghb >> 1;
            ghb = ghb + (unsigned int)(pow(2,hybrid_n-1));
           // cout<<"ghb after updating : " <<ghb<<endl;
          }
      
      if((pred_gshare =='t') && (actual_br == 'n'))
          {
           // cout<<"ghb: "<<ghb<<endl;
            ghb = ghb >> 1;
            //mispredicts++;
           // cout<<"ghb after updating : " <<ghb<<endl;
          }

      if((pred_gshare =='n') && (actual_br == 't'))
          {
           // cout<<"ghb: "<<ghb<<endl;
            ghb = ghb >> 1;
            ghb = ghb + (unsigned int)(pow(2,hybrid_n-1));
            //mispredicts++;
           // cout<<"ghb after updating : " <<ghb<<endl;
          }
      
      if((pred_gshare =='n') && (actual_br == 'n'))
          {
           // cout<<"ghb: "<<ghb<<endl;
            ghb = ghb >> 1;
           // cout<<"ghb after updating : " <<ghb<<endl;
          }
      if((gshare_correct == 1)&&(bimodal_correct == 0)) //gshare correctm bimodal incorrect
          {
          hybrid_counter.increment_prediction_count(hybrid_counter_index);
          }
      else if ((gshare_correct == 0) &&(bimodal_correct == 1)) //gshare incorrect and bimodal incorrect
        {
        hybrid_counter.decrement_prediction_count(hybrid_counter_index);
        }

    }  
       
  }    
    cout<< "OUTPUT"<<endl;
    cout<< " number of predictions: "<<total_reads<<endl;
    cout<<" number of mispredictions: " <<mispredicts<<endl;
    printf(" misprediction rate: %0.2f \%  \n ",100*((float)mispredicts/total_reads));
    if(strcmp(brpr_model,"bimodal") == 0)
    {  
        cout<<" FINAL BIMODAL CONTENTS"<<endl;  
        bimodal.pred_table.print_stats(); 
    }  
       if(strcmp(brpr_model,"gshare") == 0)
       {
         cout<<" FINAL GSHARE CONTENTS"<<endl;  
         gshare.pred_table.print_stats();
       }
       
    if(strcmp(brpr_model,"hybrid") == 0)
    {   
       cout<<" FINAL CHOOSER CONTENTS"<<endl;  
       hybrid_counter.print_stats();
       cout<<" FINAL GSHARE CONTENTS"<<endl;  
       hybrid_gshare.pred_table.print_stats();
         cout<<" FINAL BIMODAL CONTENTS"<<endl;  
       hybrid_bimodal.pred_table.print_stats();
    }  
  return 0;
}      
       
       
       
       
       
       
       
       
       
