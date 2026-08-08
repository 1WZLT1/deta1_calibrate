#ifndef __uart_serve_h
#define __uart_serve_h

#include "stdint.h"
#include "rtthread.h"
#include "at32f435_437_dma.h"
#include "queue.h"

#define DefineStream(name, _tx_size, _rx_size )                    \
				__attribute__((zero_init,aligned(8))) uint8_t name##_serial_manager_tx_buffer[_tx_size];     \
				__attribute__((zero_init,aligned(8))) uint8_t name##_serial_manager_tx_buffer2[256];         \
				__attribute__((zero_init,aligned(8))) uint8_t name##_serial_manager_rx_buffer[_rx_size];     \
				Stream_t name =                                                  \
				{                                                                \
					.tx_size    = _tx_size,                                        \
					.rx_size    = _rx_size,                                        \
					.tx_buffer  = name##_serial_manager_tx_buffer,                 \
					.tx_buffer2 = name##_serial_manager_tx_buffer2,                \
					.rx_buffer  = name##_serial_manager_rx_buffer,                 \
				};

typedef void (*CallBackHandler)(void* p);

typedef enum
{
	STREAM_READY,
	STREAM_BUSY,
	STREAM_ERROR = -1,
}Stream_Status_t;

typedef struct Stream_t
{
	uint32_t            port;
	Stream_Status_t     State;
	volatile uint8_t*   tx_buffer;
	volatile uint8_t*   tx_buffer2;
	int                 tx_size;
	int                 tx_in_offset;
	int                 tx_out_offset;
	volatile uint8_t*   rx_buffer;
	int                 rx_size;
	struct rt_semaphore rx_new;
	rt_sem_t            rx_new_ex;
	int                 rx_in_offset;
	int                 rx_out_offset;
	void*               parameter;
	CallBackHandler     sendover_callback;
	void*               sendover_param;
	dma_channel_type*   dmax;
	
	Queue_t             tx_queue;
}Stream_t;

int Stream_Tx(Stream_t* stream,uint8_t *buf, int len);

#endif
