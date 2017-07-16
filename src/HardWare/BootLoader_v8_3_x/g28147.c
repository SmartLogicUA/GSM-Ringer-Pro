/*
  CodeVisionAVR C Compiler
  (C) 2006 Taras Drozdovsky 

  ATMEL GOST28147-89 functions
*/
#include <G28147.h>

union {
  unsigned char byte[4];
  unsigned int word[2];      
  unsigned long dword;
       } R;   

union {
  unsigned char byte[4];
  unsigned int word[2];      
  unsigned long dword;
       } CM1;   

unsigned long G[2];
              
unsigned char KeyOrderE[32]={0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,7,6,5,4,3,2,1,0};
unsigned char KeyOrderD[32]={0,1,2,3,4,5,6,7,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0,7,6,5,4,3,2,1,0};

void Prost_E(unsigned long *D,unsigned long *KZU,unsigned char *K)
{
 unsigned char i,a;
 unsigned long N1,N2;
  N2=D[1];
  N1=D[0];
   for(i=0;i!=32;i++) 
    {   
        CM1.dword= N1 + KZU[KeyOrderE[i]];
        a=K[96+(CM1.byte[3]&0x0F)];
        a|=K[112+(CM1.byte[3]>>4)]; 
        R.byte[0]=a;   
        a=K[32+(CM1.byte[1]&0x0F)];
        a|=K[48+(CM1.byte[1]>>4)]; 
        R.byte[2]=a;
        a=K[(CM1.byte[0]&0x0F)];
        a|=K[16+(CM1.byte[0]>>4)]; 
        R.byte[1]=a;
        a=K[64+(CM1.byte[2]&0x0F)];
        a|=K[80+(CM1.byte[2]>>4)]; 
        R.byte[3]=a;        
        a>>=5;  
        R.dword<<=3;
        R.byte[0]|=a;
        R.dword=N2^R.dword;
        N2=N1;
        N1=R.dword;
    }
  D[0]=N2;
  D[1]=N1;
}

void Prost_D(unsigned long *D,unsigned long *KZU,unsigned char *K)
{
 unsigned char i,a;
 unsigned long N1,N2;
  N1=D[0];
  N2=D[1];
   for(i=0;i!=32;i++) 
    {   
        CM1.dword= N1 + KZU[KeyOrderD[i]];
        a=K[96+(CM1.byte[3]&0x0F)];
        a|=K[112+(CM1.byte[3]>>4)]; 
        R.byte[0]=a;                      
        a=K[32+(CM1.byte[1]&0x0F)];
        a|=K[48+(CM1.byte[1]>>4)]; 
        R.byte[2]=a;
        a=K[(CM1.byte[0]&0x0F)];
        a|=K[16+(CM1.byte[0]>>4)]; 
        R.byte[1]=a;
        a=K[64+(CM1.byte[2]&0x0F)];
        a|=K[80+(CM1.byte[2]>>4)]; 
        R.byte[3]=a;
        a>>=5;
        R.dword<<=3;
        R.byte[0]|=a;
        R.dword=N2^R.dword;
        N2=N1;
        N1=R.dword;
    }
  D[0]=N2;
  D[1]=N1;
 }            
 void  Gost_init(unsigned long *S)
 {
  G[0]=S[1];
  G[1]=S[0];  
 }
 void  Gam_cE(unsigned long *D,unsigned long *KZU,unsigned char *K,unsigned char lenght)
{                                                                                                       
  unsigned char i;
   lenght<<=1;
   for(i=0;i!=lenght;i+=2)
   {
    Prost_E(&G[0],&KZU[0],&K[0]); 
    D[i]^=G[1];
    D[i+1]^=G[0];
    G[1]=D[i]; 
    G[0]=D[i+1];
   }
}
void  Gam_cD(unsigned long *D,unsigned long *KZU,unsigned char *K,unsigned char lenght)
{                                                                                                       
  unsigned char i;
  unsigned long P[2];
   lenght<<=1;
   for(i=0;i!=lenght;i+=2)
   {
    Prost_E(&G[0],&KZU[0],&K[0]);
    P[0]=D[i+1];
    P[1]=D[i];
    D[i]^=G[1];    
    D[i+1]^=G[0];
    G[0]=P[0]; 
    G[1]=P[1];
   } 
}
void  Gam(unsigned long *D,unsigned long *KZU,unsigned char *K,unsigned long *S,unsigned char lenght)
{
 unsigned long N3,N4,G[2];
 unsigned char i;
  G[0]=S[0];
  G[1]=S[1];
  Prost_E(&G[0],&KZU[0],&K[0]);
  N3=G[0];
  N4=G[1];
  lenght<<=1;
  for(i=0;i!=lenght;i+=2)
  {     
   N3+=0x01010101UL;
   N4+=0x01010104UL;
   G[0]=N3;
   G[1]=N4;
   Prost_E(&G[0],&KZU[0],&K[0]);
   D[i]^=G[0];
   D[i+1]^=G[1];   
  } 
}                                  
void Imit(unsigned long *D,unsigned long *KZU,unsigned char *K,unsigned long *H,unsigned char lenght)
{
 unsigned char i,j,a;
 unsigned long N1=0,N2=0;   
 lenght<<=1;
  for(j=0;j!=lenght;j+=2)
  {
   N1 = D[j]^N1;
   N2 = D[j+1]^N2;
   for(i=0;i!=16;i++) 
    {   
        CM1.dword= N1 + KZU[KeyOrderE[i]];
        a=K[96+(CM1.byte[3]&0x0F)];
        a|=K[112+(CM1.byte[3]>>4)]; 
        R.byte[0]=a;
        a=K[32+(CM1.byte[1]&0x0F)];
        a|=K[48+(CM1.byte[1]>>4)]; 
        R.byte[2]=a;
        a=K[(CM1.byte[0]&0x0F)];
        a|=K[16+(CM1.byte[0]>>4)]; 
        R.byte[1]=a;
        a=K[64+(CM1.byte[2]&0x0F)];
        a|=K[80+(CM1.byte[2]>>4)]; 
        R.byte[3]=a;
        a>>=5;  
        R.dword<<=3;
        R.byte[0]|=a;
        R.dword=N2^R.dword;
        N2=N1;
        N1=R.dword;
    }
   H[0]=N1;
   H[1]=N2;              
  }     
}  
 void Sbox(unsigned char *K)
{
 unsigned char i,j;
 for(i=1;i!=9;i+=2)   
  for(j=0;j!=16;j++)
  K[(i<<4)+j]<<=4;
}                    
void Sbox_e(eeprom unsigned char *K_,unsigned char *K)
{
 unsigned char i,j;
 for(i=0;i!=4;i++)
  for(j=0;j!=16;j++) 
  {
   K[(i*32)+j]=K_[(i*16)+j]&0x0F;
   K[(i*32)+j+16]=K_[(i*16)+j]&0xF0;
  } 
}
void Key_e(eeprom unsigned long *KZU_,unsigned long *KZU)
{
 unsigned char i;
 for(i=0;i!=8;i++)
  KZU[i]=KZU_[i];
}                                                        
