#include <stdint.h>
#include <stdbool.h>

//Assembly instructions
extern void BnZer2048(uint32_t* acc);
extern void BnInc2048(uint32_t* acc);
extern bool BnShr2048(uint32_t* acc, bool bitIn); //Returns bit shifted out
extern bool BnShl2048(uint32_t* acc, bool bitIn); //Returns bit shifted out
extern void BnCpy2048(uint32_t* acc, uint32_t* value);
extern void BnOrr2048(uint32_t* acc, uint32_t* value);
extern void BnAdd2048(uint32_t* acc, uint32_t* value);
extern void BnSub2048(uint32_t* acc, uint32_t* value);
extern bool BnIse2048(               uint32_t* value);
extern bool BnIne2048(               uint32_t* value);
extern int  BnCmp2048(uint32_t* lhs, uint32_t* rhs);

extern void BnZer1024(uint32_t* acc);
extern void BnInc1024(uint32_t* acc);
extern bool BnShr1024(uint32_t* acc, bool bitIn); //Returns bit shifted out
extern bool BnShl1024(uint32_t* acc, bool bitIn); //Returns bit shifted out
extern void BnCpy1024(uint32_t* acc, uint32_t* value);
extern void BnOrr1024(uint32_t* acc, uint32_t* value);
extern void BnAdd1024(uint32_t* acc, uint32_t* value);
extern void BnSub1024(uint32_t* acc, uint32_t* value);
extern bool BnIse1024(               uint32_t* value);
extern bool BnIne1024(               uint32_t* value);
extern int  BnCmp1024(uint32_t* lhs, uint32_t* rhs);

extern void BnZer512 (uint32_t* acc);
extern void BnInc512 (uint32_t* acc);
extern bool BnShr512 (uint32_t* acc, bool bitIn); //Returns bit shifted out
extern bool BnShl512 (uint32_t* acc, bool bitIn); //Returns bit shifted out
extern void BnCpy512 (uint32_t* acc, uint32_t* value);
extern void BnOrr512 (uint32_t* acc, uint32_t* value);
extern void BnAdd512 (uint32_t* acc, uint32_t* value);
extern void BnSub512 (uint32_t* acc, uint32_t* value);
extern bool BnIse512 (               uint32_t* value);
extern bool BnIne512 (               uint32_t* value);
extern int  BnCmp512 (uint32_t* lhs, uint32_t* rhs);

// Text functions
extern void BnParseHex2048(uint32_t* n, const char* p);
extern void BnAsHttp2048  (uint32_t* n);
extern void BnParseHex1024(uint32_t* n, const char* p);
extern void BnAsHttp1024  (uint32_t* n);

// Arithmetic operations
extern void Bn1024Mul2048(uint32_t* a1024, uint32_t* b1024, uint32_t* r2048);
extern void BnRem1024(int numSize, uint32_t* n, uint32_t* d1024, uint32_t* r1024);

extern void Bn512Mul1024(uint32_t* a512, uint32_t* b512, uint32_t* r1024);
extern void BnRem512(int numSize, uint32_t* n, uint32_t* d512, uint32_t* r512);

//Asynchronous operations
extern void BnModExpStart1024  (uint32_t m[32], uint32_t e[32], uint32_t n[32], uint32_t r[32], int messageSize, uint32_t* message, uint32_t* exponent, uint32_t* modulus);
extern bool BnModExpIterate1024(uint32_t m[32], uint32_t e[32], uint32_t n[32], uint32_t r[32]); //Returns true if finished
extern void BnModExpStart512   (uint32_t m[16], uint32_t e[16], uint32_t n[16], uint32_t r[16], int messageSize, uint32_t* message, uint32_t* exponent, uint32_t* modulus);
extern bool BnModExpIterate512 (uint32_t m[16], uint32_t e[16], uint32_t n[16], uint32_t r[16]); //Returns true if finished

