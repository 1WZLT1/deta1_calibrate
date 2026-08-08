#ifndef __QUEUE_H
#define __QUEUE_H
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus
#include "stdint.h"
typedef struct
{
	int                 Size;
	int                 InOffset;
	int                 OutOffset;
	int                 Length;
	uint8_t*            Buffer;
	int                 LostCount;
}Queue_t;

void Queue_Init(Queue_t* queue, uint8_t* buffer, int size);
int Queue_Input(Queue_t* queue, uint8_t* buffer,int length);
int Queue_Output(Queue_t* queue, uint8_t* buffer,int max_size);
int Queue_Available(Queue_t* queue);

#ifdef __cplusplus
}
#endif //__cplusplus
#endif //__QUEUE_H
