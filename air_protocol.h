#ifndef __AIR_PROTOCOL_H__
#define __AIR_PROTOCOL_H__
#include <stdint.h>

#define AIR_TAG	0x5A6B7C8D

typedef enum
{
	AIR_SUCCESS,
	AIR_FAIL,
}air_ret_t;

typedef struct
{
	uint32_t tag;
	uint16_t cmd;
	uint16_t len;
} air_header_t;

typedef struct
{
	uint16_t cmd;
	uint8_t *pdata;
	uint16_t len;
} air_result_t;

#define AIR_HEADER_LEN	sizeof(air_header_t)

air_ret_t air_alloc_pack(uint16_t cmd,uint8_t *pdata,uint16_t len,air_result_t *result);
air_ret_t air_check_pack(uint8_t *pdata,uint16_t len);
air_ret_t air_data_parser(uint8_t *pdata,uint16_t len,air_result_t *result);
void air_show_log(uint8_t *pdata,uint16_t len);

#endif