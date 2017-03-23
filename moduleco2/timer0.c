      #include <iom128.h>
      #include <ina90.h>
      #include <stdio.h>
      #include <string.h>
      #include "delay.h"
      #include "timer0.h"  
     

      void timer0_init()
      {

            TCCR0 = 0x07;     //타이머 동작을 설정하고 프리스케일러를 설정.
                              //0x00 : No clock source (stop)
                              //0x01 : No prescaling
                              //0x02 : clk / 8
                              //0x03 : clk / 32 
                              //0x04 : clk / 64
                              //0x05 : clk / 128 (used)
                              //0x06 : clk / 256
                              //0x07 : clk / 1024

            TCNT0 = 249;
            TIMSK = 0x01;
            TIFR |= 0x01;

      }
