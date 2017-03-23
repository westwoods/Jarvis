#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include "usart_128.h"
      unsigned char zg01cv_data_bank[10];
      unsigned char zg01cv_data_bank_copy[10];      
      int counter = 0;
      int condition =0;
      int co2_value = 0;
      int co2_flag = 0;
      int co2 = 0;
      unsigned char co2_bank[10];

      int temp_value = 0;
      int temp_flag = 0;
      float temp = 0.0;
      unsigned char temp_bank[10];

      unsigned char serial_in;
      unsigned int humi = 0;      
      unsigned int humi_value = 0;
      unsigned int humi_flag = 0;
      
      int x = 0;
      int rx_start_flag = 0;
      
            unsigned char asc2hex(unsigned char asc)
      {
            unsigned char ret = 0;
            
            if(asc >= '0' && asc <= '9')
            {
                  ret = asc - 0x30;
            } 
            else if(asc >= 'A' && asc <= 'F')
            {
                  ret = asc - 0x41 + 10;
            }

            return ret;
      }
      
      
      void zg01cv_rx_data_process(unsigned char rx_co2_data)
      {
            unsigned char got_data = 0;
            float f_temp = 0.0;
            
            got_data = rx_co2_data;
           // printf ("%3X", got_data) ;  // 데이터 받는 함수

            if((rx_start_flag == 0) && (got_data == 0x02))//start
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

                        zg01cv_data_bank_copy[0];
                        zg01cv_data_bank_copy[1];
                        zg01cv_data_bank_copy[2];
                        zg01cv_data_bank_copy[3];
                        zg01cv_data_bank_copy[4];
                        zg01cv_data_bank_copy[5];
                        zg01cv_data_bank_copy[6];   
                        zg01cv_data_bank_copy[7];


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
	                      printf("습도 %d \"%\"",humi);
                              printf("온도 %f C' ",temp);
                              printf("이산화 %d PPM\n",co2);
			if(co2>=2000&&condition==0){
				system("cvlc ./co2007.m4a -q&");
				condition =1;
				}
			else if(condition ==1 && co2<1200){
				condition = 0;
				system("cvlc ./co2008.m4a -q&"); 

			}
                              humi_flag = 0;
                              co2_flag = 0;
                              temp_flag = 0;
                        }

                        rx_start_flag = 0;
                  }
            }
      }

int main ()
{
  int fd ;
  int count ;
  unsigned int nextTime ;
  unsigned int nowtime;
  if ((fd = serialOpen ("/dev/ttyAMA0",19200 )) < 0)  // 두번째 인자값이 보레이트 설정
  {
    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;                   
    return 1 ;
  }
 
  if (wiringPiSetup () == -1)
  {
    fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;
    return 1 ;
  }
 
  nextTime = millis () + 300 ;
 // printf("%d",nextTime);
  for (count = 0 ;  ; )
  {
    nowtime = millis();
  //  printf("%d",nowtime);
    if (nowtime > nextTime)
    {
    //  printf ("\nOut: %d: ", count) ;
      fflush (stdout) ;
      //serialPutchar (fd, count) ; // 데이터 전송해주는 함수
      nextTime += 300 ;
      ++count ;
    }
 
    delay (200) ;
 
    while (serialDataAvail (fd))
    {
		serial_in=serialGetchar (fd);
		zg01cv_rx_data_process(serial_in);
        fflush (stdout) ;
    }
  }
 
  printf ("\n") ;
  return 0 ;
}
 
