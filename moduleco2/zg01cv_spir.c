

//------------------------------------------------------------
// include header file
//------------------------------------------------------------
      #include <iom128.h>
      #include <ina90.h>
      #include <stdio.h>
      #include <string.h>
      #include "delay.h"
      #include "usart_128.h"
      #include "zg01cv_spir.h"

      extern unsigned char str[30];

      int cnt1 = 0;
      int cnt2 = 0;
      int i_check_sum = 0;
      unsigned char zg01cv_tw_data = 0;
      unsigned char sensor_data[5]={0,0,0,0,0};
      unsigned char uc_check_sum = 0;
      unsigned int ui_temperature = 0;
      unsigned int ui_co2 = 0;
      unsigned int ui_humidity = 0.0;
      
      float f_temperature = 0.0;
      

      
      
      void zg01cv_spir_read(unsigned char input_data)
      {
            unsigned char in_data = 0;
            int z = 0;

            in_data = input_data;

            if(in_data == 0x08)  zg01cv_tw_data |=  0x01;
            else                 zg01cv_tw_data &=~(0x01);

            if(cnt1 >= 7)
            {
                  cnt1 = 0;
                  sensor_data[cnt2] = zg01cv_tw_data;
                  zg01cv_tw_data = 0;

                  if(cnt2 >= 4)
                  {
                        if(
                           ((sensor_data[0] == 0x41) && (sensor_data[4] == 0x0D))|| // Humidity
                           ((sensor_data[0] == 0x42) && (sensor_data[4] == 0x0D))|| // Temperature
                           ((sensor_data[0] == 0x50) && (sensor_data[4] == 0x0D))  // Co2
                          )
                        {
                              i_check_sum = 0;

                              for(z = 0; z<= 2; z++) //for checksum
                              {
                                    i_check_sum += sensor_data[z];
                              }
                              uc_check_sum = (i_check_sum & 0xFF);

                              if(uc_check_sum == sensor_data[3]);
                              {
                                    for(z = 0; z <= 4; z++)
                                    {
                                          hex_to_ascii(sensor_data[z]);
                                          //string_out0(" ");
                                    }
                                    
                                    string_out0(" ");

                                    if(sensor_data[0] == 0x41) //Humidity calculation
                                    {
                                          string_out0("HUMI : ");
                                          ui_humidity  = sensor_data[1];
                                          ui_humidity  = (ui_humidity << 8);
                                          ui_humidity |= sensor_data[2];
                                          memset(str,0,sizeof(str));
                                          decimal_to_ascii_for_unsign_4digit(ui_humidity,str);
                                          string_out0(str);
                                          string_out0("%RH\n\r");
                                          memset(sensor_data,0,sizeof(sensor_data));                                          
                                    }
                                    
                                    else if(sensor_data[0] == 0x42) //Temperature calculation
                                    {
                                          string_out0("TEMP : ");
                                          ui_temperature  = sensor_data[1];
                                          ui_temperature  = (ui_temperature << 8);
                                          ui_temperature |= sensor_data[2];
                                          f_temperature = (float)ui_temperature;
                                          f_temperature = (f_temperature / 16.0) - 273.15;
                                          float2asc_3_2(f_temperature,str);
                                          string_out0(str);
                                          string_out0("'C\n\r");
                                          memset(sensor_data,0,sizeof(sensor_data));                                          
                                    }
                                    else if(sensor_data[0] == 0x50) //Co2
                                    {
                                          string_out0("Co2  :  ");
                                          ui_co2  = sensor_data[1];
                                          ui_co2  = (ui_co2 << 8);
                                          ui_co2 |= sensor_data[2];

                                          memset(str,0,sizeof(str));
                                          decimal_to_ascii_for_unsign_4digit(ui_co2,str);
                                          string_out0(str);
                                          string_out0(" PPM\n\r");
                                          
                                          memset(sensor_data,0,sizeof(sensor_data));
                                    }
                                    else
                                    {
                                          memset(sensor_data,0,sizeof(sensor_data));
                                    }
                              }
                        }
                        cnt2 = 0;
                  }
                  else cnt2++;
            }
            else
            {
                  cnt1++;
                  zg01cv_tw_data = (zg01cv_tw_data << 1);
            }
      }
