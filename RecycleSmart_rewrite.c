/******************************
Author: Cann John Gundogdu
		Sample code for RecycleSmart	11/11/2021

******************************/



#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "RecycleSmart_rewrite.h"


const char* const conn_setup = "AT+CONNECT\r\n";// This modem command should not be changed.Therefore,it should be located
												// into read only memory .rodata . If it would be located into data segment
												// it may be overwritten or broken.

const char* const conn_check = "AT+CHECK\r\n";	// located .rodata

const char* send_data = "AT+DATA = 01234567890123456789";	// it can be located into data segment.
															// With a const char* it is immutable

volatile unsigned int NUM_OF_TRANSMIT_TRIAL = 3;			// if uart_tx() returns FAIL, send_message() tries to send again current packet (NUM_OF_TRANSMIT_TRIAL) 3 times.
															// this variable is volatile because I don't want any optimization by the compiler on it.
															// any other thread or process such as administration process can edit it.
															// if you want to only 1 time trial for uart_tx() you can initialize it to 1.
															// Do not initialize it to 0.



/**@brief Transmits bytes out the serial UART port. Can only transmit 20 bytes at a time.
 *
 * @param data (input only) Const char* to data to be sent.Data is immutable with this pointer.
 * @param size (input/output) for an input parameter size is the length (number of bytes to be sent) of data to be sent.
 *              it is updated with the Number of bytes successfully was sent on return.
 * @retval SUCCESS  If transaction was successful. Size param updated to valid Tx'd size.(Number of bytes successfully was sent)
 * @retval FAIL  If transaction failed. Size param invalid if it failed (assume 0 bytes sent).
 */

ret_t uart_tx(const char* data,size_t* size);


/*
 * @brief send_message() handles how to transmit the message via UART by calling uart_tx().
 *        send_message splits the message into packets.Each packet has (MAX_NUM_OF_BYTES_TO_TRANSMIT) 20 bytes at most.
 *        The last or only one packet (if the length of data less than (MAX_NUM_OF_BYTES_TO_TRANSMIT)) packet can be smaller size.
 *        It sends message packet by packet by calling uart_tx().
 *        uart_tx() can return SUCCESS and number of bytes actually was sent.
 *        send_message() rearranges the packets again according with the number of bytes actually was sent.
 *        uart_tx() can return FAIL too.
 *        if uart_tx() returns FAIL send_message tries to send again current packet (NUM_OF_TRANSMIT_TRIAL) 3 times.
 *        if any packet can not be sent in 3 times send_message() returns FAIL.
 *        if all packets has been sent successfully send_message() returns SUCCESS.
 *
 * @param message (input only) to data to be sent. with a const char * it is immutable.
 * @retval SUCCESS  If message has been sent successfully.
 * @retval FAIL  If transaction failed.
 */

ret_t send_message(const char* message)
{
	size_t i;
	int remain_size,index,transmit_trial_cnt;

	remain_size = strlen(message);
	index = 0;

	for (i = LENGTH_OF_PACKET; remain_size >= LENGTH_OF_PACKET; remain_size -= i,index += i )
	{
		for (transmit_trial_cnt = NUM_OF_TRANSMIT_TRIAL; transmit_trial_cnt > 0; --transmit_trial_cnt)
		{
			i = LENGTH_OF_PACKET;
			if (uart_tx(&message[index],&i) == SUCCESS) break;
		}
		if (transmit_trial_cnt == 0) return FAIL;
	}

	if (remain_size > 0 )
	{
		for (i = remain_size; remain_size > 0; remain_size -= i,index += i )
		{

			for (transmit_trial_cnt = NUM_OF_TRANSMIT_TRIAL; transmit_trial_cnt > 0; transmit_trial_cnt--)
			{
				i = remain_size;
				if (uart_tx(&message[index],&i) == SUCCESS) break;
			}
			if (transmit_trial_cnt == 0) return FAIL;
		}
	}
	return SUCCESS;
}

/*
 * @brief 	transmit_next_msg() transmits data via UART by calling send_message() depending on the state param.
 *  		Depending on the state, it should transmit the respective message.
 *
 * @param 	state (input only) for selecting respective message.
 * @retval 	SUCCESS  If message has been sent successfully.
 * @retval 	FAIL  If transaction failed.
 */

ret_t transmit_next_msg(state_t state)
{
	switch(state)
	{
	case STATE_CONN_SETUP:
		return send_message(conn_setup);
		break;
	case STATE_CONN_CHECK:
		return send_message(conn_check);
		break;
	case STATE_SEND_DATA:
		return send_message(send_data);
		break;
	default:
		return FAIL;
		break;
	}
}
