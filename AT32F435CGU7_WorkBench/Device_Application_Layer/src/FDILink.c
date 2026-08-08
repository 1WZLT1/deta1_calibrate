#include "fdilink.h"
#include "verify_tool.h"
#include <stdint.h>

int FDILink_Pack(uint8_t* buffer, FDILink_t* FDILink, uint8_t type, void* buf, int len)
{
	buffer[FDILink_Frame_Start] = FDILink_STX_Flag;
	buffer[FDILink_Frame_CMD] = type;
	buffer[FDILink_Frame_Length] = len;
	buffer[FDILink_Frame_SerialNumber] = FDILink->TxNumber++;
	uint8_t CRC8 = CRC8_Table(buffer, FDILink_Frame_CRC8);
	buffer[FDILink_Frame_CRC8] = CRC8;
	if(len == 0)
	{
		//没有CRC16校验和结束符
		return FDILink_Frame_CRC8 + 1;
	}
	else
	{
		uint8_t* buf_data = buffer + FDILink_Frame_Data;
		//memcpy(buf_data,buf,len);
		for(int i = 0;i < len;i++)
		{
			buf_data[i] = ((uint8_t*)buf)[i];
		}
		uint16_t CRC16 = CRC16_Table(buf_data, len);
		buffer[FDILink_Frame_CRC16H] = (CRC16 >> 8);
		buffer[FDILink_Frame_CRC16L] = (CRC16 & 0xff);
		buffer[FDILink_Frame_End + len - 1] = FDILink_EDX_Flag;
		return FDILink_Frame_End + len;
	}
}
