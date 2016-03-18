#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
int main(int argc,char **argv)
{
  ifstream input(argv[1]);
  FILE *f;//file pointer for BACKING_STORE.bin file
  string s;//string to read virtual addresses
  TLB tlb;
  PT page_table;
  PM phy_memory;
  int num_tran=0;//record number of translation
  while(getline(input,s))//read one line from address file
  {
    num_tran++;
    stringstream ss;
    int vaddr,paddr,value;
    char op='0';
    if(s[s.size()-1]=='W')
    {
      op='W';
      s=s.substr(0,s.size()-2);
    }
    else if(s[s.size()-1]=='R')
    {
      op='R';
      s=s.substr(0,s.size()-2);
    }
    ss<<s;
    ss>>vaddr;//change the address into int type
    int vpn,offset,ppn;
    offset=vaddr%(1<<8);
    vpn=(vaddr>>8)%(1<<8);
    if(tlb.find_ppn(vpn,ppn))//find physical page number in TLB
    {
      if(op=='R'||op=='0')
	value=phy_memory.read(ppn,offset);//read the value from physical memory
      else if(op=='W')
	value=phy_memory.write(ppn,offset);//write a value to physical memory
    }
    else if(page_table.find_ppn(vpn,ppn))//find physical page number in page table
    {
      if(op=='R'||op=='0')
	value=phy_memory.read(ppn,offset);//read the value from physical memory
      else if(op=='W')
	value=phy_memory.write(ppn,offset);//write a value to physical memory
      tlb.update_TLB(vpn,ppn);//add new connection into TLB
    }
    else//get value from bin
    {
      ppn=phy_memory.find_LRU();
      page_table.update_table(vpn,ppn);//add new connection into page table
      tlb.update_TLB(vpn,ppn);//add new connection into TLB
      char v[256];
      f=fopen("BACKING_STORE.bin","r");
      if(phy_memory.if_dirty(ppn))//check if this frame of physical memory is dirty, if so, swap back this frame to BACKING_STORE
      {
	int back_vpn;
	back_vpn=phy_memory.get_origin_vpn(ppn);//get the place in BACKING_STORE.bin file to swap the frame
	fseek(f,back_vpn*256,SEEK_SET);
	for(int i=0;i<256;i++)
	  v[i]=phy_memory.get_one_number(ppn,i);
	fwrite(v,256,1,f);
      }
      fseek(f,vpn*256,SEEK_SET);
      fread(v,256,1,f);
      phy_memory.add(vpn,ppn,v);
      if(op=='R'||op=='0')
	value=phy_memory.read(ppn,offset);
      else if(op=='W')
	value=phy_memory.write(ppn,offset);
      fclose(f);
    }
    paddr=(ppn<<8)+offset;
    //cout<<paddr<<endl;
    cout<<"Virtual address: "<<vaddr<<" Physical address: "<<paddr<<" Value: "<<value<<endl;
  }
  cout<<"Number of Translated Addresses = "<<num_tran<<endl;
  cout<<"Page fault rate is: "<<page_table.get_fault_rate()*100<<"%"<<endl;
  cout<<"TLB hit rate is: "<<tlb.get_hit_rate()*100<<"%"<<endl;
  input.close();
  return 0;
}
