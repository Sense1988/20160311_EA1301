
#ifndef __MTKBT_HMAC_SHA2__
#define __MTKBT_HMAC_SHA2__

/*
#define uchar   unsigned char
#define uint    unsigned int
#define ulong   unsigned long int
*/
#define HASH_BLOCK_SIZE					64
#define HASH_BLOCK_SIZE_MASK	0x3F

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
    unsigned int total[2];
    unsigned int state[8];    /* states are the hash values */
    unsigned char buffer[64]; /* buffers are for 64 message schedule */
}sha2_context;

void LM_SspEng_sha2_hmac(unsigned char *buf, unsigned int buflen, unsigned char *key, unsigned int keylen, unsigned char *digest);

#ifdef __cplusplus
}
#endif

#endif /* __MTKBT_HMAC_SHA2__ */
