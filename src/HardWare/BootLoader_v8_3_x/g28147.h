/*
  CodeVisionAVR C Compiler
  (C) 2006 Taras Drozdovsky, My.
*/                 
#ifndef _GOST28147_INCLUDED_
#define _GOST28147_INCLUDED_

#pragma used+
void Prost_E(unsigned long *D,unsigned long *KZU,unsigned char *K);
void Prost_D(unsigned long *D,unsigned long *KZU,unsigned char *K);
void Gam_cE(unsigned long *D,unsigned long *KZU,unsigned char *K,unsigned char lenght);
void Gam_cD(unsigned long *D,unsigned long *KZU,unsigned char *K,unsigned char lenght);
void Gam(unsigned long *D,unsigned long *KZU,unsigned char *K,unsigned long *S,unsigned char lenght);
void Imit(unsigned long *D,unsigned long *KZU,unsigned char *K,unsigned long *H,unsigned char lenght);
void Sbox(unsigned char *K);
void Sbox_e(eeprom unsigned char *K_,unsigned char *K);
void Key_e(eeprom unsigned long *KZU_,unsigned long *KZU);
void Gost_init(unsigned long *);
#pragma used-
#endif
