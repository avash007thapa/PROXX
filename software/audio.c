#include "audio.h"
#include "sprite_dev.h"


void audio_play(unsigned char* track, int track_size) {
//	while(true){
//    u16* ptr = (u16) track;
//
//    int num_samples = track_size / sizeof(u16);
//
//    // Loop through each sample
//    for (int i = 0; i < num_samples; i++) {
//        usleep(23);
//
//        int16_t sample_16b =((int16_t*)ptr);
//        int32_t sample_32b = ((int32_t)sample_16b) << 6;
//
//        Xil_Out32(I2S_DATA_TX_L_REG, sample_32b);
//
//        Xil_Out32(I2S_DATA_TX_R_REG, sample_32b);
//
//        ptr++;
//
//
//    }
//    if (XUartPs_IsReceiveData(UART_BASEADDR)) {
//	   if (XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET) == 'q') {
//		   return;
//	   }
//    }
//
//    xil_printf("End of audio playback\r\n");
}
