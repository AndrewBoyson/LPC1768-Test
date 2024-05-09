extern void PrfHmacSha256(uint8_t* secret, int      secretLength,
                          uint8_t* seed,   int      seedLength,
                          int number,      uint8_t* output); //output needs to hold number * 32 bytes


extern void PrfHmacSha256Test(void);