/**
 ****************************************************************************************
 *
 * @file user_circ.h
 *
 * @brief User circular buffer header file.
 *
 * Copyright (c) 2015-2018 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor.
 *
 * By using this Software you agree that Dialog Semiconductor retains all
 * intellectual property and proprietary rights in and to this Software and any
 * use, reproduction, disclosure or distribution of the Software without express
 * written permission or a license agreement from Dialog Semiconductor is
 * strictly prohibited. This Software is solely for use on or in conjunction
 * with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, THE
 * SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE
 * PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, IN NO EVENT SHALL
 * DIALOG SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THE SOFTWARE.
 *
 ****************************************************************************************
 */

#ifndef _USER_CIRC_BUFF_H_
#define _USER_CIRC_BUFF_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "stdint.h"
#include "stdbool.h"


#define CIRC_BUFF_TYPE          char

#define CIRC_BUFF_DEF(x,y)                \
   __RETAINED CIRC_BUFF_TYPE x##_data_space[y];            \
    __RETAINED circ_buff_t x = {                     \
        .buffer = x##_data_space,         \
        .head = 0,                        \
        .tail = 0,                        \
        .maxlen = y                       \
    }
		
typedef struct {
    char * const buffer;
    int head;
    int tail;
    const int maxlen;
} circ_buff_t;


typedef enum{
	
	CIRC_BUFF_ERROR_NO_ERROR,
	CIRC_BUFF_BUFFER_FULL,
	CIRC_BUFF_BUFFER_EMPTY,
	
}circ_buff_error_t;

typedef enum{
	
	CIRC_BUFF_IS_EMPTY,
	CIRC_BUFF_IS_FULL,
	CIRC_BUFF_DATA_AVAIL
	
}circ_buff_status_t;

uint32_t circ_buff_get_num_items(circ_buff_t *c);

circ_buff_status_t circ_buff_get_status(circ_buff_t *c);

circ_buff_error_t circ_buff_pop(circ_buff_t *c, CIRC_BUFF_TYPE *data);

circ_buff_error_t circ_buff_push(circ_buff_t *c, CIRC_BUFF_TYPE data);

void circ_buff_clear_data(circ_buff_t *c);

circ_buff_error_t circ_buff_pop_all(circ_buff_t *c, CIRC_BUFF_TYPE *data, uint32_t *len);

circ_buff_error_t circ_buff_push_buffer(circ_buff_t *c, CIRC_BUFF_TYPE *buffer, uint32_t size);


#endif // _USER_CIRC_BUFF_H_
