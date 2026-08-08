#ifndef __FDILinkManager_h
#define __FDILinkManager_h

#include "stdint.h"
#include "uart_serve.h"

extern int busy_flag;
extern int count;
void FDILinkManager_Init(void);
void FDILinkSend_RAWData(uint64_t time);

#endif

