
      #include <iom128.h>
      #include <ina90.h>
      #include <stdio.h>
      #include "delay.h"

     
     
      // When cristal use 7.3728MHz, delay is msec
      void delay_ms(unsigned int msec)       
      { 
            while(msec--) __delay_cycles(7373);
      }
    
         

      // When cristal use 7.3728MHz, delay is usec
      void delay_us(unsigned int usec)       
      { 
            while(usec--) __delay_cycles(1);
      }
               
    
    
      void delay(unsigned int k) 
      {
            while(k) k--;
      }
              
    
    
      void delay10(unsigned int k) 
      {
            int i;
            unsigned int j;
      
            j = k;
      
            for(i=0;i<10;i++)
            {
                  while(j--); 
                  j = k;
            }
      }
