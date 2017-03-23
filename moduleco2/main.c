      //2015.12.1
      //Program by I.G.Jeon / Nanosentech., ltd.
      //MCU : ATMEGA128A
      //INPUT : DC5V



      #include <iom128.h>
      #include <ina90.h>
      #include <stdio.h>
      #include <string.h>
      #include "delay.h"
      #include "port.h"
      #include "usart_128.h"
      #include "zg01cv_spir.h"
      #include "zg01cv_uart.h"
      #include "external_interrupt.h"
      #include "timer0.h"
      #include "adc.h"

      #define uart_mode 1
      #define spir_mode 2
      #define vout_mode 3

      extern unsigned char zg01cv_spir_data;
      extern unsigned char str[30];
      
      int mode = 1;
      unsigned char rx_1_data = 0;

      int interrupt_data = 0;
      int interrupt_ready = 0;
      int interrupt_ready_flag = 0;
      int ms = 0;
      
      unsigned char zg01cv_data = 0;
      int zg01cv_flag = 0;

      unsigned char interrupt_rx_data = 0;
      
      int sensor_ready_flag = 0;
      unsigned char check_data = 0;
      int check_counter = 0;
      
      unsigned int ui_v_co2 = 0;
      float f_v_co2 = 0.0;
      
      unsigned int ui_v_temp = 0;
      float f_v_temp = 0.0;
      
    

      #pragma vector = TIMER0_OVF_vect
      __interrupt void TIMER0_OVERFLOW_interrupt(void)
      {
            if(mode == spir_mode)
            {
                  if(sensor_ready_flag == 0) //each 10ms check
                  {
                        check_data = PINE;
                        check_data = (check_data & 0x18);
                  
                        if(check_data == 0x18)
                        {
                              if(check_counter >= 7)
                              {
                                    check_counter = 0;
                                    sensor_ready_flag = 1;
                              }
                              else check_counter++;
                        }
                        else check_counter = 0;
                  }
            }
            
                         //7.3728MHz
            TCNT0 = 188; //10ms = 188
                         // 1ms = 249
      }

      #pragma vector = USART1_RXC_vect
      __interrupt void UART1_RXC(void)
      {
            if(mode == uart_mode)
            {
                  rx_1_data = UDR1;
                  zg01cv_rx_data_process(rx_1_data);
            }
      }
      
      #pragma vector = INT4_vect
      __interrupt void INT4_interrupt(void)
      {   
            if(mode == spir_mode)
            {            
                  if(sensor_ready_flag == 1)
                  {
                        interrupt_rx_data = (PINE & 0x08);
                        zg01cv_spir_read(interrupt_rx_data);
                  }
            }
      }

   
      
      void main()
      {
            _CLI();
            
            port_init(); 
            external_interrupt_init();            
            usart0_init();
            usart1_init();
            timer0_init();
            adc_init();
            adc_channel_selection(0);
            
            _SEI();
            
            //--------------------------------------------------------
            // Sensor input mode selection
            
            //mode = spir_mode;  
            mode = uart_mode;
            //mode = vout_mode;
            //--------------------------------------------------------
            
            while(1)
            {
                  if(mode == vout_mode)
                  {
                        adc_channel_selection(0);                        
                        ui_v_co2 = 0;
                        
                        ui_v_co2 = read_adc(5);
                        
                        memset(str,0,sizeof(str));
                        decimal_to_ascii_for_unsign_4digit(ui_v_co2,str);
                        string_out0(str);
                        string_out0("ADC ");                        
                        
                        f_v_co2 = (float)ui_v_co2;
                        f_v_co2 = (f_v_co2 * (5000.0/1024.0));
                        ui_v_co2 = (unsigned int)f_v_co2;
                        
                        memset(str,0,sizeof(str));
                        decimal_to_ascii_for_unsign_4digit(ui_v_co2,str);
                        string_out0(str);
                        string_out0("mV ");
                        
                        memset(str,0,sizeof(str));
                        decimal_to_ascii_for_unsign_4digit(ui_v_co2,str);
                        string_out0(str);
                        string_out0("PPM     ");

                        //TEMPERATURE
                        adc_channel_selection(1);                        
                        ui_v_temp = 0;
                        
                        ui_v_temp = read_adc(5);
                        memset(str,0,sizeof(str));
                        decimal_to_ascii_for_unsign_4digit(ui_v_temp,str);
                        string_out0(str);
                        string_out0("ADC ");
                        
                        f_v_temp = (float)ui_v_temp;
                        f_v_temp = (f_v_temp * (5000.0/1024.0));
                        ui_v_temp = (unsigned int)f_v_temp;
                        
                        memset(str,0,sizeof(str));
                        decimal_to_ascii_for_unsign_4digit(ui_v_temp,str);
                        string_out0(str);
                        string_out0("mV ");

                        f_v_temp = (f_v_temp / 50.0);
                        memset(str,0,sizeof(str));
                        float2asc_3_2(f_v_temp,str);
                        string_out0(str);
                        string_out0("'C\n\r");                        
                        
                        delay_ms(30);                        
                  }
                  
                  
            }
      }
