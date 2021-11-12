# RecycleSmart_rewrite

Hi Kris,

I rewrote your sample code and would like to tell you something about the new code.

I assume the ret_t uart_tx(const char* data,size_t* size) like that
    parameter data (input only) Const char* to data to be sent.
    parameter size (input/output) for an input parameter size is the length (number of bytes to be sent) of data to be sent.
              it is updated with the Number of bytes successfully sent on return.
    return value   SUCCESS  If transaction was successful. Size param updated to valid Tx'd size.(Number of bytes successfully sent)
    return value   FAIL  If transaction failed. Size param invalid if it failed (assume 0 bytes sent).
 
I located the modem commands to rodata. They do not need to change and they should stay as they are. 
Of course they can be located to data segment too. But I prefer to locate them to read only memory.

I located send_data[] to data segment.

I create a variable named NUM_OF_TRANSMIT_TRIAL which is initialized to 3.
When sending a data packet via uart_tx(), if it fails, send_message() tries to send same packet 3 times.
I made it volatile for no optimization by compiler.
I thought that this can be changed by administration process or any other thread to increase or decrease trial times on runtime. 


I wrote a function send_message() which is called by transmit_next_msg().
    parameter message (input only) to data to be sent. with a const char * it is immutable.
    return value SUCCESS  If message has been sent successfully.
    return value FAIL  If transaction failed.

    send_message() handles how to transmit the message via UART by calling uart_tx().
    send_message() splits the message into packets.Each packet has (MAX_NUM_OF_BYTES_TO_TRANSMIT) 20 bytes maximum.
    The last or only packet (if the length of data less than (MAX_NUM_OF_BYTES_TO_TRANSMIT))  can be a smaller size.
    It sends message packet by packet by calling uart_tx().
    uart_tx() can return SUCCESS and number of bytes actually sent.
    send_message() rearranges the packets again according to the number of bytes actually sent.
    uart_tx() can return FAIL too.
    if uart_tx() returns FAIL send_message tries to send again current packet (NUM_OF_TRANSMIT_TRIAL) 3 times.
    if any of the packets can not be sent in 3 times send_message() returns FAIL.
    if all packets have been sent successfully send_message() returns SUCCESS.


I rewrote transmit_next_message()
    parameter 	state (input only) for selecting respective message.
    return value SUCCESS  If message has been sent successfully.
    return value FAIL  If transaction failed. 
    transmits data via UART by calling send_message() depending on the state param.


Best Regards

Cann John
