#include "queue.h"
#include "string.h"
#include "at32f435_437.h"

void Queue_Init(Queue_t* queue, uint8_t* buffer, int size)
{
	queue->Buffer = buffer;
	queue->Size = size;
	queue->InOffset = 0;
	queue->OutOffset = 0;
	queue->Length = 0;
	queue->LostCount = 0;
}

static inline void Queue_Put(Queue_t* queue, uint8_t value)
{
	queue->Buffer[queue->InOffset] = value;
	queue->InOffset = (queue->InOffset + 1) % queue->Size;
}

int Queue_Input(Queue_t* queue, uint8_t* buffer, int length)
{
	int primask = __get_PRIMASK();
	__set_PRIMASK(1);
	if(queue->Length + length > queue->Size)
	{
		queue->LostCount += length;
		__set_PRIMASK(primask);
		return -1;
	}
	queue->Length += length;
	for(int i = 0;i < length;i++)
		Queue_Put(queue, buffer[i]);
	__set_PRIMASK(primask);
	return 0;
}

static int Queue_Read(Queue_t* queue)
{
	int value = -1;
	int primask = __get_PRIMASK();
	__set_PRIMASK(1);
	if (queue->Length)
	{
		value = queue->Buffer[queue->OutOffset];
		queue->OutOffset = (queue->OutOffset + 1) % queue->Size;
		queue->Length--;
	}
	__set_PRIMASK(primask);
	return value;
}
int Queue_Available(Queue_t* queue)
{
	return queue->Length;
}
int Queue_Output(Queue_t* queue, uint8_t* buffer, int max_size)
{
	int primask = __get_PRIMASK();
	__set_PRIMASK(1);
	int length = queue->Length;
	__set_PRIMASK(primask);
	if(length > max_size)
		length = max_size;
	for(int i = 0;i < length;i++)
		buffer[i] = Queue_Read(queue);
	return length;
}
