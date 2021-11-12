
#define LENGTH_OF_PACKET  20								// uart transmission cand send only 20 bytes at a time.

typedef enum
{
	SUCCESS = 0,
	FAIL
}ret_t;


typedef enum
{
	STATE_CONN_SETUP = 0,	// Set up connection
	STATE_CONN_CHECK,		// Check connection
	STATE_SEND_DATA			// Send data
}state_t;
