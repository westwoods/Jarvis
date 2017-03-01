
      #include <iom128.h>
      #include <ina90.h>
      #include <stdio.h>
      #include <string.h>
      #include "usart_128.h"
      #include "zg01cv_uart.h"

      extern unsigned char str[30];

      unsigned char zg01cv_data_bank[10];
      unsigned char zg01cv_data_bank_copy[10];      
      int counter = 0;

      int co2_value = 0;
      int co2_flag = 0;
      int co2 = 0;
      unsigned char co2_bank[10];

      int temp_value = 0;
      int temp_flag = 0;
      float temp = 0.0;
      unsigned char temp_bank[10];

      unsigned int humi = 0;      
      unsigned int humi_value = 0;
      unsigned int humi_flag = 0;
      
      int x = 0;
      int rx_start_flag = 0;
      
      
      
      
      void zg01cv_rx_data_process(unsigned char rx_co2_data)
      {
            unsigned char got_data = 0;
            float f_temp = 0.0;
            
            got_data = rx_co2_data;
            
            if((rx_start_flag == 0) && (got_data == 0x02))//<STX>
            {
                  memset(zg01cv_data_bank,0,sizeof(zg01cv_data_bank));
                  counter = 0;
                  
                  rx_start_flag = 1;
            }

            else if(rx_start_flag == 1)
            {
                  zg01cv_data_bank[counter] = got_data;
                  counter++;
                  
                  if(counter >=8)
                  {
                        memset(zg01cv_data_bank_copy,0,sizeof(zg01cv_data_bank));
                        memcpy(zg01cv_data_bank_copy,zg01cv_data_bank,sizeof(zg01cv_data_bank));
                        memset(zg01cv_data_bank,0,sizeof(zg01cv_data_bank));

                        counter = 0;

                        putch0(zg01cv_data_bank_copy[0]);
                        putch0(zg01cv_data_bank_copy[1]);
                        putch0(zg01cv_data_bank_copy[2]);
                        putch0(zg01cv_data_bank_copy[3]);
                        putch0(zg01cv_data_bank_copy[4]);
                        putch0(zg01cv_data_bank_copy[5]);
                        putch0(zg01cv_data_bank_copy[6]);   
                        putch0(zg01cv_data_bank_copy[7]);


                        if((zg01cv_data_bank_copy[0] == 'P') && (zg01cv_data_bank_copy[7] == 0x0D))
                        {
                              co2_value = 0;
                              co2_value |= asc2hex(zg01cv_data_bank_copy[1]) << 12;
                              co2_value |= asc2hex(zg01cv_data_bank_copy[2]) << 8;
                              co2_value |= asc2hex(zg01cv_data_bank_copy[3]) << 4;
                              co2_value |= asc2hex(zg01cv_data_bank_copy[4]) << 0;
                              co2 = co2_value;
                              
                              co2_flag = 1; 
                        }
                        else if((zg01cv_data_bank_copy[0] == 'B') && (zg01cv_data_bank_copy[7] == 0x0D))
                        {
                              temp_value = 0;
                              temp_value |= asc2hex(zg01cv_data_bank_copy[1]) << 12;
                              temp_value |= asc2hex(zg01cv_data_bank_copy[2]) << 8;
                              temp_value |= asc2hex(zg01cv_data_bank_copy[3]) << 4;
                              temp_value |= asc2hex(zg01cv_data_bank_copy[4]) << 0;
                              
                              f_temp = (float)temp_value;
                              f_temp = (f_temp / 16) - 273.15;
                              
                              temp = f_temp;
                              
                              temp_flag = 1;                                
                        }
                        else if((zg01cv_data_bank_copy[0] == 'A') && (zg01cv_data_bank_copy[7] == 0x0D))
                        {
                              humi_value = 0;
                              humi_value |= asc2hex(zg01cv_data_bank_copy[1]) << 12;
                              humi_value |= asc2hex(zg01cv_data_bank_copy[2]) << 8;
                              humi_value |= asc2hex(zg01cv_data_bank_copy[3]) << 4;
                              humi_value |= asc2hex(zg01cv_data_bank_copy[4]) << 0;
                              humi = humi_value;
                              
                              humi_flag = 1;                                
                        }                        
                        
                        if((co2_flag == 1) && (temp_flag == 1) && (humi_flag == 1))
                        {
                              memset(str,0,sizeof(str));
                              decimal_to_ascii_for_unsign_4digit(humi,str);
                              string_out0(str);
                              string_out0("%RH  ");
                              
                              memset(str,0,sizeof(str));                          
                              float2asc_3_2(temp,str);
                              string_out0(str);
                              string_out0("'C  ");                              
                              
                              memset(str,0,sizeof(str));
                              decimal_to_ascii_for_unsign_4digit(co2,str);
                              string_out0(str);
                              string_out0("PPM\n\r");
                              
                              humi_flag = 0;
                              co2_flag = 0;
                              temp_flag = 0;
                        }

                        rx_start_flag = 0;
                  }
            }
      }
