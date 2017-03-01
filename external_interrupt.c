      #include <iom128.h>
      #include <ina90.h>
      #include <stdio.h>
      #include <string.h>
      #include "delay.h"
      #include "external_interrupt.h"


    
      void external_interrupt_init()
      {
            /*
            // External Interrupt(s) initialization
            // INT0: On
            // INT0 Mode: Falling Edge
            // INT1: Off
            // INT2: Off
            GICR  |= 0x40;
            MCUCR  = 0x02;
            MCUCSR = 0x00;
            GIFR   = 0x40;
            
            TIMSK  = 0x01;
            */
            
            // External Interrupt(s) initialization
            // INT0: Off
            // INT1: Off
            // INT2: Off
            // INT3: Off
            // INT4: On
            // INT4 Mode: Falling Edge
            // INT5: Off
            // INT6: Off
            // INT7: Off
            EICRA = 0x00;
            EICRB = 0x02;
            EIMSK = 0x10;
            EIFR = 0x10;   
            
            TIMSK  = 0x01;
      }
