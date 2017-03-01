
#ifndef __usart_128_h_
#define __usart_128_h_

     void usart0_init(void); 
     void usart1_init(void);      
     
     char getch0(void);
     char getch1(void);     
     
     void putch0(unsigned char udr0_data);
     void putch1(unsigned char udr1_data);
 
     void string_out0(unsigned char *str);
     void string_out1(unsigned char *str); 
         
   
     void decimal_to_ascii_for_sign_4digit(int aValue, unsigned char *aStr);
     void decimal_to_ascii_for_unsign_4digit(int aValue, unsigned char *aStr);   
     void decimal_to_ascii_for_unsign_1digit(int aValue, unsigned char *aStr);        
     
     void hex_to_ascii(unsigned char ch);
     void hex_to_int_ascii(unsigned int ch);

     void pa_usart1_s_4digit(int aValue); 
     void pa_usart1_s_5digit(int aValue,unsigned char *aStr);     
     unsigned char SF04_CheckCrc (unsigned char data[], unsigned char nbrOfBytes, unsigned char checksum);     
     void float2asc_3_2(float aValue, unsigned char *aStr);
     void float2asc_0(float aValue, unsigned char *aStr);
     
     unsigned char asc2hex(unsigned char asc);

#endif
     
