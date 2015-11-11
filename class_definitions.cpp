#include <iostream>

typedef unsigned long ulong;


class BTB_block
{
  public:
    bool valid;
    ulong tag;
    unsigned int count;
    
    BTB_block()
    {
      valid = 0;
      tag = 0;
      count = 2;
    }

};

class BTB
{
  public:
    BTB_block * btb_block;
    unsigned int size;
    BTB(){};
    void BTB_c(unsigned int M1)
    {
      size = M1;
      btb_block = new BTB_block[M1];
    }
  
    int find_tag_in_BTB(ulong, ulong);
    void fill_tag_in_BTB(ulong, ulong);
    unsigned int  get_prediction_count(ulong);
    void increment_prediction_count(ulong);
    void decrement_prediction_count(ulong);
    void print_stats();
    void set_count_to_1();
};

int BTB::find_tag_in_BTB (ulong tag, ulong index)
{
  int pos;
  int hit = 0;

    if((tag == btb_block[index].tag) && (btb_block[index].valid == 1))
    {
      hit =1;
      
    }
  return hit;  
}

void BTB::fill_tag_in_BTB(ulong tag, ulong index)
{
  btb_block[index].tag = tag;
  btb_block[index].valid = 1;
}

unsigned int  BTB::get_prediction_count(ulong index)
{
  return btb_block[index].count;
}

void BTB::increment_prediction_count(ulong index)
{
  if(btb_block[index].count<3)
  {
    btb_block[index].count++;
  }
}

void BTB::decrement_prediction_count(ulong index)
{
  if(btb_block[index].count>0)
  {
    btb_block[index].count--;
  }
}

void BTB::print_stats()
{
  for(int i = 0; i<size;i++)
  {
    std::cout<<std::dec<<i<<" "<<get_prediction_count(i)<<std::endl;
  }
}

void BTB::set_count_to_1()
{
  for(int i=0;i<size;i++)
  {
    btb_block[i].count = 1;
  }
}

class Bimodal
{
  public:
  BTB pred_table;

  Bimodal(){};
 void  Bimodal_c(unsigned int M2)
  {
    pred_table.BTB_c(M2);
  }

};

class Gshare
{
  public:
    BTB pred_table;
    unsigned int ghb;
    void Gshare_c(unsigned int size)
    {
      pred_table.BTB_c(size);
      ghb = 0;
    }
};
