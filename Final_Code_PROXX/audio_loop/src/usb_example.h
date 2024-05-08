#ifndef USB_EXAMPLE_H
#define USB_EXAMPLE_H

#include <stdbool.h>
#include <unistd.h>
#include "xscugic.h"
#include "xil_printf.h"
#include "xusbps.h"
#include "xusbps_endpoint.h"
#include "xusbps_hw.h"

#include "usb.h"
#include "sprite_dev.h"

extern XScuGic INTCInst;
extern UsbWithHost usbWithHostInstance;
extern XUsbPs *UsbInstancePtr;

void updateMousePosition(int *wasX, int *wasY, int newX, int newY);
void updateMouseClick(bool isLMBIn, bool isRMBIn);
void UsbIntrHandler(void *CallBackRef, u32 Mask);

#endif
