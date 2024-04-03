// LPC1768 Vectors
// ===============

.syntax unified
.thumb

.section ".vectors"
.long _stack
.long main              //Reset_Handler
.long DefaultHandler    //NMI_Handler
.long HardFaultHandler  //HardFault_Handler
.long DefaultHandler    //MemManage_Handler
.long DefaultHandler    //BusFault_Handler
.long DefaultHandler    //UsageFault_Handler
.long 0
.long 0
.long 0
.long 0
.long DefaultHandler    //SVC_Handler
.long DefaultHandler    //DebugMon_Handler
.long 0
.long DefaultHandler    //PendSV_Handler
.long DefaultHandler    //SysTick_Handler
.long DefaultHandler    //WDT_IRQHandler
.long DefaultHandler    //TIMER0_IRQHandler
.long OneWireBusHandler //TIMER1_IRQHandler
.long DefaultHandler    //TIMER2_IRQHandler
.long DefaultHandler    //TIMER3_IRQHandler
.long DefaultHandler    //UART0_IRQHandler
.long DefaultHandler    //UART1_IRQHandler
.long DefaultHandler    //UART2_IRQHandler
.long DefaultHandler    //UART3_IRQHandler
.long DefaultHandler    //PWM1_IRQHandler
.long DefaultHandler    //I2C0_IRQHandler
.long DefaultHandler    //I2C1_IRQHandler
.long DefaultHandler    //I2C2_IRQHandler
.long DefaultHandler    //SPI_IRQHandler
.long DefaultHandler    //SSP0_IRQHandler
.long DefaultHandler    //SSP1_IRQHandler
.long DefaultHandler    //PLL0_IRQHandler
.long DefaultHandler    //RTC_IRQHandler
.long DefaultHandler    //EINT0_IRQHandler
.long DefaultHandler    //EINT1_IRQHandler
.long DefaultHandler    //EINT2_IRQHandler
.long DefaultHandler    //EINT3_IRQHandler
.long DefaultHandler    //ADC_IRQHandler
.long DefaultHandler    //BOD_IRQHandler
.long DefaultHandler    //USB_IRQHandler
.long DefaultHandler    //CAN_IRQHandler
.long DefaultHandler    //DMA_IRQHandler
.long DefaultHandler    //I2S_IRQHandler
.long DefaultHandler    //ENET_IRQHandler
.long RitHandler        //RIT_IRQHandler
.long DefaultHandler    //MCPWM_IRQHandler
.long DefaultHandler    //QEI_IRQHandler
.long DefaultHandler    //PLL1_IRQHandler

.end
