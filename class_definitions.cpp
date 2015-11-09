#include <iostream>

typedef unsigned long ulong;


class BTB_block
{
  public:
    bool valid;
    ulong tag;
    
    BTB_block()
    {
      valid = 0;
      tag = 0;
    }

}

class BTB
{
  public:
    BTB_block * btb_block;

    BTB();
    BTB(int M1)
    {
      btb_block = new btb_block[M1];
    }



