
#ifndef __adc_h
#define __adc_h

      void adc_init();
      unsigned int read_adc(unsigned char filter_cycle);
      unsigned char adc_channel_selection(unsigned char ch_number);
      unsigned int slp_33a_auto_zero_process(unsigned int check_cnt);      
      
#endif
     
