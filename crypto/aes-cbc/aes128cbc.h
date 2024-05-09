#pragma once

#include <stdint.h>

#define AES128CBC_BLOCK_SIZE 16 //Block length in bytes AES is 128b block only

void Aes128CbcEncrypt(const uint8_t* key, const uint8_t* iv, uint8_t* buf, uint32_t length);
void Aes128CbcDecrypt(const uint8_t* key, const uint8_t* iv, uint8_t* buf, uint32_t length);
