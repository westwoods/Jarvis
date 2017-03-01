      #include <iom128.h>
      #include <ina90.h>
      #include <stdio.h>
      #include <string.h>
      #include <float.h>
      #include "adc.h"
      #include "delay.h"
      #include "usart_128.h"
      
      int adc_done_flag = 0;
      extern unsigned char str[30];         

      
      void adc_init()
      {

            // ADC initialization
            // ADC Clock frequency: 115.200 kHz
            // ADC Voltage Reference: Int., cap. on AREF
            // ADC Auto Trigger Source: Free Running
            ADMUX  = 0xc0;
            ADCSR = 0xA7;
            ADCSR |= 0x40;
      } 


      unsigned int read_adc(unsigned char filter_cycle)
      {
            unsigned int adc_result = 0;
            unsigned int adc_count = 0;
            unsigned int adc_sum = 0;
            unsigned int adc_return_value = 0;

            for(adc_count = 1; adc_count <= filter_cycle; adc_count++)                 
            {
                  while((ADCSR & 0x10)==0);
                        
                  ADCSR |= 0x10;
                  
                  adc_result  =  ADCL;
                  adc_result |= (ADCH<<8);
                  adc_sum += adc_result;
                  adc_result = 0;
            }
            
            adc_return_value = (adc_sum / (filter_cycle));
            
            adc_sum = 0;
            
            adc_done_flag = 1;
       
            return adc_return_value;          
      }
      


      unsigned char adc_channel_selection(unsigned char ch_number)
      {
            unsigned char mux_value = 0;

            ADMUX  = 0x00;
            ADCSR = 0x00;
            delay_ms(1);
            
            switch(ch_number)
            {
                  case 0 : mux_value = 0x00;
                           break;
                           
                  case 1 : mux_value = 0x01;
                           break;
                           
                  case 2 : mux_value = 0x02;
                           break;
                           
                  case 3 : mux_value = 0x03;
                           break;
                           
                  case 4 : mux_value = 0x04;
                           break;
                           
                  case 5 : mux_value = 0x05;
                           break;
                           
                  case 6 : mux_value = 0x06;
                           break;
                           
                  case 7 : mux_value = 0x07;
                           break;       
                           
                  default : string_out1("adc_channel_mux_error\n\r");
                            break;                                                                                                                                                           
            }
            
            ADMUX  = (0xc0|mux_value);
            ADCSR = 0xA7;
            ADCSR |= 0x40;
            delay_ms(1);
            
            //return mux_value;
            return 0;
      }
