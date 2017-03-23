
      #include <iom128.h>
      #include <ina90.h>
      #include <stdio.h>
      #include <string.h>
      #include "delay.h"
      #include "usart_128.h"

      unsigned char str[30];
      

      
//------------------------------------------------------------                  
      void usart0_init(void)
//------------------------------------------------------------       
      {
            UCSR0A = 0x00;
            UCSR0B = 0x98;            
            UCSR0C = 0x06;
            UBRR0H = 0x00;
            UBRR0L = 23; 
            //47 = 9600bps
            //23 = 19200bps
     }

//------------------------------------------------------------                  
      void usart1_init(void)
//------------------------------------------------------------       
      {
            UCSR1A = 0x00;
            UCSR1B = 0x98;            
            UCSR1C = 0x06;
            UBRR1H = 0x00;
            UBRR1L = 23;  //9600
     }
     
     

     
     
//------------------------------------------------------------ 
      void string_out0(unsigned char *str) //문자열 출력 함수 
//    ex) string_out("abcd1234");
//------------------------------------------------------------       
      {
            unsigned int i = 0;
             
            do
            {
                  putch0(str[i]);
            }
             
             while(str[++i]!='\0');   // NULL 문자를 만날 때 까지
      }
      
//------------------------------------------------------------ 
      void string_out1(unsigned char *str) //문자열 출력 함수 
//    ex) string_out("abcd1234");
//------------------------------------------------------------       
      {
            unsigned int i = 0;
             
            do
            {
                  putch1(str[i]);
            }
             
             while(str[++i]!='\0');   // NULL 문자를 만날 때 까지
             
             //memset(str,0,sizeof(str));
      }      
                         




//------------------------------------------------------------ 
      char getch0(void)// 시리얼 포트로부터 한문자를 받는다.  
//    ex) 앞서 선언했던 변수 = getch();         
//------------------------------------------------------------      
      {
            unsigned char udr0_data;
     
            while(!(UCSR0A & 0x80));
             
            udr0_data = UDR0;
     
            UCSR0A |= 0x80;        // 클리어 RXC0
             
            return(udr0_data);
     }
     
//------------------------------------------------------------ 
      char getch1(void)// 시리얼 포트로부터 한문자를 받는다.  
//    ex) 앞서 선언했던 변수 = getch();         
//------------------------------------------------------------      
      {
            unsigned char udr1_data;
     
            while(!(UCSR1A & 0x80));
             
            udr1_data = UDR1;
     
            UCSR1A |= 0x80;        // 클리어 RXC1
             
            return(udr1_data);
     }     




//------------------------------------------------------------ 
      void putch0(unsigned char udr0_data)// 시리얼 포트로 한문자를 보낸다. 
//------------------------------------------------------------      
      {
            while(!(UCSR0A & 0x20));
             
            UDR0= udr0_data;
         
            UCSR0A |= 0x20;         // 클리어 UDRE0
      }
            
            
//------------------------------------------------------------ 
      void putch1(unsigned char udr1_data)// 시리얼 포트로 한문자를 보낸다. 
//------------------------------------------------------------      
      {
            while(!(UCSR1A & 0x20));
             
            UDR1 = udr1_data;
         
            UCSR1A |= 0x20;         // 클리어 UDRE0
      }            




/*
//------------------------------------------------------------ 
     void decimal_to_ascii_for_sign_4digit(int aValue, unsigned char *aStr)              
//------------------------------------------------------------      
     {
          int data = 0;
      
          
          if(aValue < 0)
          {
                 aStr[0] = '-';
                 aValue  = (aValue * (-1)); 

          } else {
              
                 aStr[0] = '+';
                 }

              data = aValue / 100;
              aStr[1] = data + 0x30;
              data = data * 100;
              aValue = aValue - data;
              data = aValue / 100;     

              data = aValue / 10;              
              aStr[2] = data + 0x30;
              data = data * 10;
              aValue = aValue - data;

              aStr[3] = aValue + 0x30;      
      }
*/
      
      
//------------------------------------------------------------ 
     void float2asc_3_2(float aValue, unsigned char *aStr)              
//------------------------------------------------------------      
      {
            float float_msb = 0.0;
            float float_lsb = 0.0;
            int data  = 0;
            int i_msb = 0;
            int i_lsb = 0;
    
            if(aValue >= 0.0)
            {
                  aStr[0] = '+';
            } 
            else
            {
                  aStr[0] = '-';
                  aValue  = (aValue * (-1.0));                  
            }
            
            i_msb = (int)aValue;
            float_msb = (float)i_msb;

            float_lsb = (aValue - float_msb);
            i_lsb = (int)(float_lsb * 100);

              data = i_msb / 100;
              aStr[1] = data + 0x30;
              data = data * 100;
              i_msb = i_msb - data;
              
              data = i_msb / 10;
              aStr[2] = data + 0x30;
              data = data * 10;
              i_msb = i_msb - data;              

              data = i_msb / 1;
              aStr[3] = i_msb + 0x30;
              
              aStr[4]= '.';
              
              data = i_lsb / 10;
              aStr[5] = data + 0x30;
              data = data * 10;
              i_lsb = i_lsb - data;              

              data = i_lsb / 1;
              aStr[6] = i_lsb + 0x30;             
      }      
      
      


//------------------------------------------------------------ 
     void float2asc_0(float aValue, unsigned char *aStr)              
//------------------------------------------------------------      
      {
            float float_msb = 0.0;
            float float_lsb = 0.0;
            int data  = 0;
            int i_msb = 0;
            int i_lsb = 0;
            int temp = 0;
            float f_temp=0.0;
            
            temp = (int)aValue * 100;
            f_temp = (float)temp / 100;
    
            if(f_temp >= 0.0)
            {
                  aStr[0] = '+';
            } 
            else
            {
                  aStr[0] = '-';
                  aValue  = (aValue * (-1.0));                  
            }
            
            i_msb = (int)aValue;
            float_msb = (float)i_msb;

            float_lsb = (aValue - float_msb);
            i_lsb = (int)(float_lsb * 100.0);

              data = i_msb / 100;
              aStr[1] = data + 0x30;
              data = data * 100;
              i_msb = i_msb - data;
              
              data = i_msb / 10;
              aStr[2] = data + 0x30;
              data = data * 10;
              i_msb = i_msb - data;              

              data = i_msb / 1;
              aStr[3] = i_msb + 0x30;
              
              //aStr[4]= '.';
              
              data = i_lsb / 10;
              aStr[4] = data + 0x30;
              data = data * 10;
              i_lsb = i_lsb - data;              

              data = i_lsb / 1;
              aStr[5] = i_lsb + 0x30;             
      }      
      
      


      
//------------------------------------------------------------ 
     void decimal_to_ascii_for_unsign_4digit(int aValue, unsigned char *aStr)              
//------------------------------------------------------------      
      {
            int data = 0;

            data = aValue / 1000;
            aStr[0] = data + 0x30;
            data = data * 1000;
            aValue = aValue - data;              

            data = aValue / 100;
            aStr[1] = data + 0x30;
            data = data * 100;
            aValue = aValue - data;    

            data = aValue / 10;
            aStr[2] = data + 0x30;
            data = data * 10;
            aValue = aValue - data; 
            
            data = aValue / 1;
            aStr[3] = data + 0x30;          
      }      
      
//------------------------------------------------------------ 
     void decimal_to_ascii_for_unsign_1digit(int aValue, unsigned char *aStr)              
//------------------------------------------------------------      
     {
            aStr[0] = aValue + 0x30;      
     } 
     

//------------------------------------------------------------ 
      void hex_to_ascii(unsigned char ch)                      
//------------------------------------------------------------       
      {
            unsigned char H_nibble, L_nibble;
            
            L_nibble = ch & 0x0f;
            H_nibble = ch >> 4;
            
            if  (H_nibble < 0x0a) H_nibble+= 0x30;
            else                  H_nibble+= 0x37; 
            

            putch0(H_nibble);
            
            if  (L_nibble < 0x0a) L_nibble+=0x30;
            else                  L_nibble+=0x37;
            
            putch0(L_nibble);  
                  
      }
      
//------------------------------------------------------------ 
      void hex_to_int_ascii(unsigned int ch)                      
//------------------------------------------------------------       
      {
            unsigned char data = 0;   

            data = (ch >> 8);   
            hex_to_ascii(data); 
            
            data = 0;
            
            data = (ch & 0xff);
            hex_to_ascii(data);             
      } 


//------------------------------------------------------------ 
     void pa_usart1_s_4digit(int aValue)
//------------------------------------------------------------      
      {
            int data = 0;
          
            if(aValue < 0)
            {
                  putch1('-');
                  aValue  = (aValue * (-1)); 

            } 
            else
            {
                  putch1('+');
            }

            data = aValue / 100;
            putch1(data + 0x30);
            data = data * 100;
            aValue = aValue - data;
              

            data = aValue / 10;              
            putch1(data + 0x30);
            //data = data * 10;
            //aValue = aValue - data;
            
            data = aValue % 10;                

            putch1(data + 0x30);
      }
      
      
//------------------------------------------------------------ 
     void pa_usart1_s_5digit(int aValue,unsigned char *aStr)
//------------------------------------------------------------      
      {
            int data = 0;
          
            if(aValue < 0)
            {
                  aStr[0] = '-';
                  aValue  = (aValue * (-1)); 
            } 
            else
            {
                  aStr[0] = '+';
            }

            data = aValue / 10000;
            aStr[1] = data + 0x30;
            data = data * 10000;
            aValue = aValue - data;
            
            data = aValue / 1000;
            aStr[2] = data + 0x30;
            data = data * 1000;
            aValue = aValue - data;
            
            data = aValue / 100;
            aStr[3] = data + 0x30;
            data = data * 100;
            aValue = aValue - data;
            
            data = aValue / 10;
            aStr[4] = data + 0x30;
            data = data * 10;
            aValue = aValue - data;            
            
            data = aValue / 1;                
            aStr[5] = data + 0x30;
      }
      
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