#ifndef __fdilink_h
#define __fdilink_h

#include "stdint.h"

#define FDILink_Frame_Start					0
#define FDILink_Frame_CMD					  1
#define FDILink_Frame_Length				2
#define FDILink_Frame_SerialNumber	3
#define FDILink_Frame_CRC8					4
#define FDILink_Frame_CRC16H				5
#define FDILink_Frame_CRC16L				6
#define FDILink_Frame_Data					7
#define FDILink_Frame_End					  8

#define FDILink_STX_Flag 0xFC
#define FDILink_EDX_Flag 0xFD
#define FDILink_Connect_Flag 0xFD

typedef struct FDILink_Status
{
	int 				BootStatus;
	int					RxStatus;
	int 				RxType;
	int 				RxDataLeft;
	int         RxLost;
	int		 			RxNumber;
	int         RxTotalBytes;
	int         RxOKBytes;
	int		 			TxNumber;
	int 				CRC8_Verify;
	int 				CRC16_Verify;
	int64_t     LastReceiveTime;
	uint32_t 	  BufferIndex;
	uint8_t 		FDILink_Frame_Buffer[12];
	uint8_t 		Buffer[256];
}FDILink_t;

int FDILink_Pack(uint8_t* buffer, FDILink_t* FDILink, uint8_t type, void* buf, int len);

#endif

