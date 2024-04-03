#define PCONP       (*((volatile unsigned *) 0x400FC0C4))
#define PCLKSEL0    (*((volatile unsigned *) 0x400FC1A8))
#define PCLKSEL1    (*((volatile unsigned *) 0x400FC1AC))
#define PINSEL0     (*((volatile unsigned *) 0x4002C000))
#define PINSEL1     (*((volatile unsigned *) 0x4002C004))
#define PINSEL2     (*((volatile unsigned *) 0x4002C008))
#define PINSEL3     (*((volatile unsigned *) 0x4002C00C))
#define PINSEL4     (*((volatile unsigned *) 0x4002C010))
#define PINMODE4    (*((volatile unsigned *) 0x4002C050))
#define PINMODE_OD2 (*((volatile unsigned *) 0x4002C070))

void PeriphsInit (void)
{
    //Peripheral power - Table 46
    PCONP  = 0;
    PCONP |= 1 <<  1; //TIMER0
    PCONP |= 1 <<  2; //TIMER1
    PCONP |= 1 <<  3; //UART0
    PCONP |= 1 <<  6; //PWM1
    PCONP |= 1 <<  9; //RTC
    PCONP |= 1 << 10; //SSP1
    PCONP |= 1 << 15; //GPIO
    PCONP |= 1 << 16; //RIT
    PCONP |= 1 << 30; //ENET                 

    //Peripheral clock must be selected before PLL0 enabling and connecting; default is 00 divide by 4; need 01 to have divide by 1
    PCLKSEL0  = 0;
    PCLKSEL0 |= 1 <<  2;  //TIM0
    PCLKSEL0 |= 1 <<  4;  //TIM1
    PCLKSEL0 |= 1 <<  6;  //UART0
    PCLKSEL0 |= 1 << 12;  //PWM1
    PCLKSEL0 |= 1 << 20;  //SSP1

    //Pin functions table 80.
    PINSEL0  = 0;
    PINSEL0 |= 1U <<  4; //P0.02 01 TXD0  UART0
    PINSEL0 |= 1U <<  6; //P0.03 01 RXD0  UART0
    PINSEL0 |= 2U << 14; //P0.07 10 SCK1   SSP1
    PINSEL0 |= 2U << 16; //P0.08 10 MISO1  SSP1
    PINSEL0 |= 2U << 18; //P0.09 10 MOSI1  SSP1
    
    PINSEL1  = 0;
    
    PINSEL2  = 0;
    PINSEL2 |= 1U <<  0; //P1.00 01 ENET_TXD0
    PINSEL2 |= 1U <<  2; //P1.01 01 ENET_TXD1
    PINSEL2 |= 1U <<  8; //P1.04 01 ENET_TX_EN
    PINSEL2 |= 1U << 16; //P1.08 01 ENET_CRS
    PINSEL2 |= 1U << 18; //P1.09 01 ENET_RXD0
    PINSEL2 |= 1U << 20; //P1.10 01 ENET_RXD1
    PINSEL2 |= 1U << 28; //P1.14 01 ENET_RX_ER
    PINSEL2 |= 1U << 30; //P1.15 01 ENET_REF_CLK
    
    PINSEL3  = 0;
    PINSEL3 |= 1U <<  0; //P1.16 01 ENET_MDC
    PINSEL3 |= 1U <<  2; //P1.17 01 ENET_MDIO
    
    PINSEL4  = 0;
    PINSEL4 |= 1U << 0;  //P2.00 01 PWM1.1 --> p26
}
