#include "user_circ_buff.h"
#include <stdio.h>
#include <string.h>

uint32_t circ_buff_get_num_items(circ_buff_t *c)
{

        if(c->head >= c->tail)
        {
                return  (c->head - c->tail);
        }else
        {
                return ((c->maxlen - c->tail) + c->head);
        }
}

circ_buff_error_t circ_buff_push_buffer(circ_buff_t *c, CIRC_BUFF_TYPE *buffer, uint32_t size)
{

        int to_end_len;

        uint32_t space_avail = c->maxlen - circ_buff_get_num_items(c) - 1;
        if(space_avail < size)
        {
                return CIRC_BUFF_BUFFER_FULL;
        }

        to_end_len = c->maxlen - c->head;

        if(to_end_len >= size)
        {
                memcpy((char *)&c->buffer[c->head], buffer, size);
                c->head = c->head + size;
                return CIRC_BUFF_ERROR_NO_ERROR;
        }else
        {
             memcpy((char *)&c->buffer[c->head], buffer, to_end_len);
             int remaining_bytes = size - to_end_len;
             memcpy((char *)&c->buffer[0], (char *)&buffer[to_end_len], remaining_bytes);
             c->head = remaining_bytes;
        }

        return CIRC_BUFF_ERROR_NO_ERROR;
}

circ_buff_error_t circ_buff_push(circ_buff_t *c, CIRC_BUFF_TYPE data)
{
    int next;
		
	
    next = c->head + 1;  // next is where head will point to after this write.
    if (next >= c->maxlen)
        next = 0;

    if (next == c->tail)  // if the head + 1 == tail, circular buffer is full
    {
        return CIRC_BUFF_BUFFER_FULL;
    }

    c->buffer[c->head] = data;  // Load data and then move
    c->head = next;             // head to next data offset.  
		
		
    return CIRC_BUFF_ERROR_NO_ERROR;  // return success to indicate successful push.
}


circ_buff_error_t circ_buff_pop(circ_buff_t *c, CIRC_BUFF_TYPE *data)
{
    int next;

	
    if (c->head == c->tail){
            // if the head == tail, we don't have any data
        return CIRC_BUFF_BUFFER_EMPTY;
    }

    next = c->tail + 1;  // next is where tail will point to after this read.
    if(next >= c->maxlen)
        next = 0;

    *data = c->buffer[c->tail];  // Read data and then move
    c->tail = next;              // tail to next offset.
		
		
    return CIRC_BUFF_ERROR_NO_ERROR;  // return success to indicate successful push.
}

circ_buff_error_t circ_buff_pop_all(circ_buff_t *c, CIRC_BUFF_TYPE *data, uint32_t *len)
{
	
	*len =0;
	circ_buff_error_t ret = CIRC_BUFF_ERROR_NO_ERROR;
	
	do
	{
		ret = circ_buff_pop(c, data + *len);	
		//case when buffer is empty, don't increment so we know nothing was popped
		if(!ret)
			*len+=1;
		
	} while ( ret == CIRC_BUFF_ERROR_NO_ERROR );
	
	return ret;
	
	
}

circ_buff_status_t circ_buff_get_status(circ_buff_t *c)
{
	
	if (c->head == c->tail)  // if the head == tail, we don't have any data
        return CIRC_BUFF_IS_EMPTY;
	
	if (c->head + 1 == c->tail)  // if the head + 1 == tail, circular buffer is full
        return CIRC_BUFF_IS_FULL;
	
	return CIRC_BUFF_DATA_AVAIL;
	
	
}

void circ_buff_clear_data(circ_buff_t *c)
{
	
	c->head = 0;
	c->tail = 0;
	
	memset(c->buffer, 0, c->maxlen);
	
}

/// @} APP


