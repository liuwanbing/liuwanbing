#ifndef _ENCRYPTION_H
#define _ENCRYPTION_H

#ifndef uint8
#define uint8  unsigned char
#endif

#ifndef uint32
#define uint32 unsigned long int
#endif

typedef struct
{
	uint32 erk[64];     /* encryption round keys */
	uint32 drk[64];     /* decryption round keys */
	int nr;             /* number of rounds */
}
context;

int  set_key( context *ctx, uint8 *key, int nbits );
void encrypt( context *ctx, uint8 input[16], uint8 output[16] );
void decrypt( context *ctx, uint8 input[16], uint8 output[16] );

void encryptPW(unsigned char *input, unsigned int inputLen,unsigned char digest[16]);
#endif
