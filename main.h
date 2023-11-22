#include <stdio.h>
#include <string.h>

typedef unsigned char byte;
typedef unsigned long long lg;

//8bit ROR,ROL(순환 시프트)
#define ROR(w,mv) ((w>>mv)|(w<<(8-mv))) 
#define ROL(w,mv) ((w<<mv)|(w>>(8-mv)))


//PIPO-128 기준 정보
//INPUT, OUTPUT: 64-BIT(8-BYTE)
//MASTER KEY:128-BIT
//ROUND KEY:64-BIT
//ROUND:13

//Function Prototype
void AddRoundKey(byte* input, byte* rk);
void S_Layer(byte* X);
void R_Layer(byte* X);
void S_Inv_Layer(byte* X);
void R_Inv_Layer(byte* X);
void pipo_ks(byte* mk, byte rk[14][8]);
void Pipo_Enc(byte* pt, byte* mk, byte* ct);
void Pipo_Dec(byte* ct, byte* mk, byte* result);
void printstate(byte* data);