
      #include <iom128.h>
      #include <ina90.h>
      #include <stdio.h>
      #include "port.h"
      
      void port_init()
      {
            DDRA  = 0xFF;
            PORTA = 0x00;
            
            DDRB  = 0xFF;
            PORTB = 0x00;
            
            DDRC  = 0xFF;
            PORTC = 0x00;
            
            DDRD  = 0xFB;
            PORTD = 0x00;
            
            DDRE  = 0x02;
            PORTE = 0x00;
            
            DDRF  = 0x00;
            PORTF = 0x00;            

            DDRG  = 0x1F;
            PORTG = 0x00;   
      }

     
