// Anonymous Cryptographic Homebrew Encryption (because I sure as hell am not claiming responsibility for this)
#include <stdio.h>  /* printf, NULL */
#include <stdlib.h> /* srand, rand */
#include <time.h>   /* time */
#include <SHA256.h>
#include <AES.h>
#include <Crypto.h>
#define SCAN 5
#define PAPER 4
#define ROCK 3
#define SCISSORS 2

#define CHACHA20_IMPLEMENTATION  //had every intention to do this, not enough time
#include "01a_ChaCha20.h"

uint8_t key[] = {
  0x00,
  0x01,
  0x02,
  0x03,
  0x04,
  0x05,
  0x06,
  0x07,
  0x08,
  0x09,
  0x0a,
  0x0b,
  0x0c,
  0x0d,
  0x0e,
  0x0f,
  0x10,
  0x11,
  0x12,
  0x13,
  0x14,
  0x15,
  0x16,
  0x17,
  0x18,
  0x19,
  0x1a,
  0x1b,
  0x1c,
  0x1d,
  0x1e,
  0x1f,
};

uint8_t nonce[] = {
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x4a,
  0x00,
  0x00,
  0x00,
  0x00,
};

uint32_t count = 0x00000001;

#define uchar unsigned char
#define uint unsigned int

#define DBL_INT_ADD(a, b, c) \
  if (a > 0xffffffff - (c)) ++b; \
  a += c;
#define ROTLEFT(a, b) (((a) << (b)) | ((a) >> (32 - (b))))
#define ROTRIGHT(a, b) (((a) >> (b)) | ((a) << (32 - (b))))

#define CH(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTRIGHT(x, 2) ^ ROTRIGHT(x, 13) ^ ROTRIGHT(x, 22))
#define EP1(x) (ROTRIGHT(x, 6) ^ ROTRIGHT(x, 11) ^ ROTRIGHT(x, 25))
#define SIG0(x) (ROTRIGHT(x, 7) ^ ROTRIGHT(x, 18) ^ ((x) >> 3))
#define SIG1(x) (ROTRIGHT(x, 17) ^ ROTRIGHT(x, 19) ^ ((x) >> 10))

uint k[64] = {
  0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
  0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
  0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
  0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
  0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
  0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
  0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
  0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

void printVec(uint8_t* path) {
  for (int i = 0; i < sizeof(path) / sizeof(path[0]); i++) {
    printf("%c ", path[i]);
  }
  printf("\n");
}

uint8_t* environmentalRandomness() { //sha256(sha256(hardcoded_array))
  static uint8_t hashf[32];
  static uint8_t hashg[32];

  srand(analogRead(11));  //set to ground for more stable entropy (XD)
  unsigned char randomness[64];
  for (int i = 0; i < 64; i++) {
    randomness[i] = rand() % 256; //wasn't working, not enough time
  }
  uint8_t chartest[64] = { 0x67, 0xC6, 0x69, 0x73, 0x51, 0xFF, 0x4A, 0xEC, 0x29, 0xCD, 0xBA, 0xAB, 0xF2, 0xFB, 0xE3, 0x46, 0x7C, 0xC2, 0x54, 0xF8, 0x1B, 0xE8, 0xE7, 0x8D, 0x76, 0x5A, 0x2E, 0x63, 0x33, 0x9F, 0xC9, 0x9A, 0x66, 0x32, 0x0D, 0xB7, 0x31, 0x58, 0xA3, 0x5A, 0x25, 0x5D, 0x05, 0x17, 0x58, 0xE9, 0x5E, 0xD4, 0xAB, 0xB2, 0xCD, 0xC6, 0x9B, 0xB4, 0x54, 0x11, 0x0E, 0x82, 0x74, 0x41, 0x21, 0x3D, 0xDC, 0x87 };
  for (int i = 0; i < 64; i++) {
    randomness[i] = chartest[i]; //overwrite "random" data with a hardcoded array
  }

  SHA256 sha256;
  Hash* hash = &sha256;
  hash->reset();
  hash->update(randomness, 64);
  hash->finalize(hashg, 32);
  hash->reset();
  hash->update(hashg, 32);
  hash->finalize(hashf, 32);
  return hashf;
}

uint8_t* xorVec(uint8_t* a, uint8_t* b) {
  int size = sizeof(a) / sizeof(a[0]) - sizeof(b) / sizeof(b[0]);
  size &= (~size) >> 31;
  size += sizeof(b) / sizeof(b[0]);
  uint8_t final[size];
  for (int i = 0; i < size; i++) {
    if (sizeof(a) / sizeof(a[0]) > i && sizeof(b) / sizeof(b[0]) > i) {
      final[i] = (a[i] ^ b[i]);
    } else if (sizeof(a) / sizeof(a[0]) > i) {
      final[i] = (a[i]);
    } else {
      final[i] = (b[i]);
    }
  }
  return final;
}

void rotate(uint8_t* a, int d, int n) {
  int i, j, temp;
  for (i = 0; i < d; i++) {
    temp = a[0];
    for (j = 0; j < n - 1; j++) {
      a[j] = a[j + 1];
    }

    a[n - 1] = temp;
  }
}

String convertToString(unsigned char* a, int size) {
  int i;
  String s = "";
  for (i = 0; i < size; i++) {
    s = s + String(char(a[i]));
  }
  return s;
}

String flagDecrypt(uint8_t* encryptedFlag) { //AES_Decrypt(sha256(sha256(hardcoded_array)), flag_data);
  uint8_t* secretxorkey = environmentalRandomness();
  // an attempt to create homebrew encryption inspired by the "Cha Cha Slide". Perhaps a future challenge...

  //int rotL = 10;
  // rotate(chachavec, rotL, sizeof(chachavec) / sizeof(chachavec[0]));

  //uint8_t* secretxorkey = chachavec;
  // rotate(chachavec, 16, 32);

  //uint8_t* secretxorkey = xorVec(chachavec, rotated_chachavec);
  //rotate(chachavec, 32, 32);
  //secretxorkey = xorVec(secretxorkey, rotated_chachavec);

  //Chacha20 chacha(key, nonce);
  //chacha.crypt(secretxorkey.data(), sizeof(secretxorkey)/sizeof(secretxorkey[0]));

  static uint8_t block_out[16];
  static uint8_t block_in[16];
  for (int i = 0; i < 16; i++) {
    block_in[i] = encryptedFlag[i];
  }

  String flag;

  AES256 ctx;
  BlockCipher* cipher = &ctx;
  crypto_feed_watchdog();
  cipher->clear(); //decrypt block 1
  cipher->setKey(secretxorkey, 32);
  cipher->decryptBlock(block_out, block_in);
  flag = convertToString(block_out, 16);
  for (int i = 16; i < 32; i++) {
    block_in[i - 16] = encryptedFlag[i];
  }

  cipher->clear(); //decrypt block 2
  cipher->setKey(secretxorkey, 32);
  cipher->decryptBlock(block_out, block_in);
  flag += convertToString(block_out, 16);
  flag.trim(); //delete null bytes
  return flag;
}
