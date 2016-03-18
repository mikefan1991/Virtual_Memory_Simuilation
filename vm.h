#include <iostream>
using namespace std;

class TLB
{
 private:
  int vpage_num[16];//virtual page number
  int ppage_num[16];//physical page number
  int LRU[16];
  int maxLRU;
  int num_access;
  int num_hit;
  int num_miss;
 public:
  TLB();
  int find_LRU();//find the index of minimum LRU value
  bool find_ppn(int vpn,int &ppn);//find physical page number with a virtual page number
  void update_TLB(int vpn,int ppn);//add a new physical page number in TLB
  double get_hit_rate();
};

class PT
{
 private:
  int table[256];
  bool valid_bit[256];
  int num_access;
  int num_hit;
  int num_miss;
 public:
  PT();
  bool find_ppn(int vpn,int &ppn);//find physical page number with a virtual page number
  void update_table(int index,int ppn);//add a new physical page number
  double get_fault_rate();
};

class PM
{
 private:
  int memory[128][256];
  bool dirty[128];
  int LRU[128];
  int vpn_save[128];
  int maxLRU;
 public:
  PM();
  int find_LRU();
  int read(int line,int offset);//read data from memory
  int write(int line,int offset);//write data into memory
  void add(int vpn_temp,int index,char v[]);//add a new frame from the BACKING_STORE
  bool if_dirty(int index);//check if one frame is modified
  int get_origin_vpn(int index);//get position in BACKING_STORE
  char get_one_number(int line,int offset);//return one data in memory
};
//--------------------------------------------------------------
TLB::TLB()
{
  for(int i=0;i<16;i++)
  {
    vpage_num[i]=-1;
    ppage_num[i]=-1;
    LRU[i]=0;
  }
  maxLRU=0;
  num_access=0;
  num_hit=0;
  num_miss=0;
}
//find the least recently used entry in TLB
int TLB::find_LRU()
{
  int min=10000000,idx=0;
  for(int i=0;i<16;i++)
  {
    if(LRU[i]<min)
    {
      min=LRU[i];
      idx=i;
    }
  }
  return idx;
}
//find physical page number with a virtual page number in TLB
bool TLB::find_ppn(int vpn,int &ppn)
{
  num_access++;
  for(int i=0;i<16;i++)
  {
    if(vpage_num[i]==vpn)//virtual page number match
    {
      num_hit++;
      ppn=ppage_num[i];
      maxLRU++;
      LRU[i]=maxLRU;
      return true;
    }
  }
  num_miss++;
  return false;
}
//add a physical page number in the LRU entry of TLB
void TLB::update_TLB(int vpn,int ppn)
{
  int index=find_LRU();
  vpage_num[index]=vpn;
  ppage_num[index]=ppn;
  maxLRU++;
  LRU[index]=maxLRU;
}

double TLB::get_hit_rate()
{
  return (double)num_hit/num_access;
}
//-------------------------------------------------
PT::PT()
{
  for(int i=0;i<256;i++)
  {
    valid_bit[i]=false;
    table[i]=-1;
  }
  num_access=0;
  num_hit=0;
  num_miss=0;
}
//find physical page number with a virtual page number in page table
bool PT::find_ppn(int vpn,int &ppn)
{
  num_access++;
  if(valid_bit[vpn]==false)//check if it is a page fault
  {
    num_miss++;
    return false;
  }
  ppn=table[vpn];
  num_hit++;
  return true;
}
//add a physical page number in page table
void PT::update_table(int index,int ppn)
{
  table[index]=ppn;
  valid_bit[index]=true;
}

double PT::get_fault_rate()
{
  return (double)num_miss/num_access;
}
//---------------------------------------------------
PM::PM()
{
  for(int i=0;i<128;i++)
  {
    dirty[i]=false;
    LRU[i]=0;
    vpn_save[i]=0;
    for(int j=0;j<256;j++)
      memory[i][j]=-1;
  }
  maxLRU=0;
}
//find the least recently used frame in physical memory
int PM::find_LRU()
{
  int min=1000000000;
  int idx=0;
  for(int i=0;i<128;i++)
  {
    if(LRU[i]<min)
    {
      min=LRU[i];
      idx=i;
    }
  }
  return idx;
}
//read a value from physical memory
int PM::read(int line,int offset)
{
  maxLRU++;
  LRU[line]=maxLRU;
  return memory[line][offset];
}
//write a value to physical memory
int PM::write(int line,int offset)
{
  maxLRU++;
  LRU[line]=maxLRU;
  dirty[line]=true;//change the dirty bit of one frame
  return memory[line][offset];
}
//add new frame to physical memory
void PM::add(int vpn_temp,int index,char v[])
{
  for(int i=0;i<256;i++)
    memory[index][i]=(int)v[i];
  dirty[index]=false;
}

bool PM::if_dirty(int index)
{
  return dirty[index];
}

int PM::get_origin_vpn(int index)
{
  return vpn_save[index];
}
//get a specific value from physical memory
char PM::get_one_number(int line,int offset)
{
  return (char)memory[line][offset];
}
