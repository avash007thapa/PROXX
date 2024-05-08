/*-----------------------------------------------------------------------------/
 /	ZYNQ Usb Host mode example												   /
 /-----------------------------------------------------------------------------/
 /	Bowie Gian
 /	2023-04-12
 /	usb_example.c
 /	https://github.com/ensc452-spring-2023/nso
 /
 /	Example code to poll a USB device.
 /
 /	A full speed Human Interface Device (HID) is assumed to be the USB device.
 /	For example: a mouse or drawing tablet.
 /	A USB hub is not supported!
 /
 /	Board Jumper Setup:
 /	JP2 must be short to enable 5V output to the USB OTG port.
 /	JP3 is recommended to be short for host mode, however my testing showed no
 /	difference.
 /
 /	In this example:
 /	- the device will be assigned address 0x0C
 /	- its device, manufacturer and serial # strings will be printed to UART
 /	- it will be set to use config 1, HID idle time = 0ms and
 /	  HID protocol 0 (assuming default configs)
 /	- it will be polled on Endpoint 1 and the data received printed to UART
 /	  (little endian)
 /
 /	Known Issues:
 /	(all issues I had could be solved by replugging the USB device)
 /	- rarely, the speed is detected, but the setup packets fail to send
 /	  resulting in the strings not being printed and the setup not completing
 /	- rarely, only 1 poll is received
 /	  (I think this is caused by the XUsbPs_QH.qTDHead holding the wrong
 /	   pointer which makes USB_qTDActivateIn() activate the qTD that received
 /	   the first poll instead of the next qTD)
/-----------------------------------------------------------------------------*/

//modified by Eva Zhu 2024-04-08 to work for Dell Laser Mouse MS3220

/*--------------------------------------------------------------*/
/* Include Files												*/
/*--------------------------------------------------------------*/

#include <stdbool.h>
#include <unistd.h>
#include "xscugic.h"
#include "xil_printf.h"
#include "xusbps.h"
#include "xusbps_endpoint.h"
#include "xusbps_hw.h"

#include "usb.h"
#include "sprite_dev.h"

/*--------------------------------------------------------------*/
/* Local Variables												*/
/*--------------------------------------------------------------*/
XScuGic INTCInst;
UsbWithHost usbWithHostInstance;
XUsbPs *UsbInstancePtr;
bool isUsbSetup = false;
XUsbPs_qTD *qTDPollReceiver = 0;

int wasX, wasY, xPos, yPos = 0;
bool isL, isR, wasL, wasR = false;

// store mouse positions - mouse signals are change in x and y positions so we need to keep track of past and current positions
// also scaled for sensitivity for Dell Laser Mouse MS3220
void updateMousePosition(int *wasX, int *wasY, int newX, int newY) {
	*wasX = xPos;
	*wasY = yPos;
	xPos += (newX % 50);

	if (xPos < 0) {
		xPos = 0;
	} else if (xPos >= 1280) {
		xPos = 1280;
	}
	yPos += (newY % 50);

	if (yPos < 0) {
		yPos = 0;
	} else if (yPos > 1024) {
		yPos = 1023;
	}
}

//keep track of when mouse was clicked
void updateMouseClick(bool isLMBIn, bool isRMBIn) {
	wasL = isL;
	wasR = isR;
	isL = isLMBIn;
	isR = isRMBIn;
	if (isL && !wasL) {
		isL = true;
	}
	if (isR && !wasR) {
		isR = true;
	}
}

/*---------------------------------------------------------------/
 / Interrupt Function											 /
 /---------------------------------------------------------------/
 / This function is called when:
 / - a USB port change is detected
 / - a setup packet is sent successfully
 / - a data packet is received
 /
 / This function handles:
 / - resetting the USB device when it is first connected
 / - detecting its speed
 / - sending setup packets
 / - receiving polled data
/---------------------------------------------------------------*/
void UsbIntrHandler(void *CallBackRef, u32 Mask)
{
	// if transaction complete (setup packet sent or data packet received)
	if (Mask & XUSBPS_IXR_UI_MASK) {
		if (isUsbSetup) {
			XUsbPs_dQHInvalidateCache(usbWithHostInstance.HostConfig.QueueHead[1].pQH);
			XUsbPs_dTDInvalidateCache(qTDPollReceiver);

			// Get the buffer pointer of the qTD
			u8 *buffInput = (u8 *)XUsbPs_ReaddTD(qTDPollReceiver,
					XUSBPS_qTDANLP) - 1;

			// get mouse values and call print function
			bool isRMB = buffInput[1] & 0x00000002; //Dell Laser Mouse MS3220 right click is stored at this location
			bool isLMB = buffInput[1] & 0x00000001; //Dell Laser Mouse MS3220 left click is stored at this location
			short *changeX = (short *) (buffInput + 1);
			short *changeY = (short *) (buffInput + 3);
			updateMousePosition(&wasX, &wasY, *changeX, *changeY);
			updateMouseClick(isLMB, isRMB);
			// call interrupt function to draw cursor to VGA in sprite_dev.cc
			generate_cursor(xPos, yPos, wasX, wasY, isL, isR, wasL, wasR);

			// Activate the next qTD
			qTDPollReceiver = USB_qTDActivateIn(
				&usbWithHostInstance.HostConfig.QueueHead[1], true, 0);

			return;
		}

		// if not setup, keep running setup
		isUsbSetup = USB_SetupDevice(&usbWithHostInstance);

		xil_printf("isUsbSetup: %d", isUsbSetup);
		// if just finished setup
		if (isUsbSetup > 0) {
			// Disable the current polling qTD and reset the DT bit
			XUsbPs_pQH *pQHPoll = usbWithHostInstance.HostConfig.QueueHead[1].pQH;

			XUsbPs_dQHInvalidateCache(pQHPoll);

			u32 qTDToken = XUsbPs_ReaddQH(pQHPoll, XUSBPS_QHqTDTOKEN);
			qTDToken |= XUSBPS_qTDTOKEN_DT_MASK;
			qTDToken &= ~0xFF; // Clear active bit and errors

			XUsbPs_WritedQH(pQHPoll, XUSBPS_QHqTDTOKEN, qTDToken);
			XUsbPs_dQHFlushCache(pQHPoll);

			// Activate the next qTD
			qTDPollReceiver = USB_qTDActivateIn(
				&usbWithHostInstance.HostConfig.QueueHead[1], true, 0);

			// Enable Periodic Schedule for polling
			XUsbPs_SetBits(UsbInstancePtr, XUSBPS_CMD_OFFSET, XUSBPS_CMD_PSE_MASK);
		}

		return;
	}

	if ((XUsbPs_ReadReg(UsbInstancePtr->Config.BaseAddress, XUSBPS_PORTSCR1_OFFSET)
		& XUSBPS_PORTSCR_CCS_MASK) == 0) {
		xil_printf("Device Disconnected!\r\n\n");
		isUsbSetup = false;
		USB_RestartSetup();

		// Disable Periodic Schedule
		XUsbPs_ClrBits(UsbInstancePtr, XUSBPS_CMD_OFFSET, XUSBPS_CMD_PSE_MASK);

		// Disable Async
		XUsbPs_ClrBits(UsbInstancePtr, XUSBPS_CMD_OFFSET, XUSBPS_CMD_ASE_MASK);

		return;
	}
	xil_printf("Device Connected! \n");

	if ((XUsbPs_ReadReg(UsbInstancePtr->Config.BaseAddress, XUSBPS_PORTSCR1_OFFSET)
		& XUSBPS_PORTSCR_PE_MASK) == 0) {
		xil_printf("Resetting...\r\n\n");
		USB_RestartSetup();

		// Reset USB device
		XUsbPs_SetBits(UsbInstancePtr, XUSBPS_PORTSCR1_OFFSET,
				XUSBPS_PORTSCR_PR_MASK);
		return;
	}

	u32 deviceSpeed =
		(XUsbPs_ReadReg(UsbInstancePtr->Config.BaseAddress,
			XUSBPS_PORTSCR1_OFFSET)
			& XUSBPS_PORTSCR_PSPD_MASK) >> 28;
	//original value was 26. A shift of 28 is needed to read device speed of Dell Laser Mouse MS3220

	if (deviceSpeed == 0) {
		xil_printf("Device operating at Full Speed.\r\n\n");
	} else if (deviceSpeed == 1) {
		xil_printf(
			"Device operating at Low Speed. Not supported - ignoring!\r\n\n");
		return;
	} else if (deviceSpeed == 2) {
		xil_printf(
			"Device operating at High Speed. Not supported - ignoring!\r\n\n");
		return;
	} else if (deviceSpeed == 3) {
		xil_printf("Device not connected.\r\n\n");
		return;
	}

	isUsbSetup = USB_SetupDevice(&usbWithHostInstance);
	xil_printf("Set-up USB.\n");
	// Enable Async for setup packets
	XUsbPs_SetBits(UsbInstancePtr, XUSBPS_CMD_OFFSET, XUSBPS_CMD_ASE_MASK);
	xil_printf("Enable Async.\n");
}

