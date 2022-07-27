#include "air_protocol.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

air_ret_t air_alloc_pack(uint16_t cmd,uint8_t *pdata,uint16_t len,air_result_t *result)
{
	air_header_t air_header;	
	uint8_t *pair_offset;
	uint8_t check_sum;
	uint16_t i;
	
	air_header.tag = AIR_TAG;
	air_header.cmd = cmd;
	air_header.len = len;
	
	result->pdata = (uint8_t *)malloc(AIR_HEADER_LEN+len+1);
	if(result->pdata == NULL)
		return AIR_FAIL;
	pair_offset = result->pdata;
	memcpy(pair_offset,&air_header,AIR_HEADER_LEN);
	pair_offset += AIR_HEADER_LEN;
	if(len > 0)
		memcpy(pair_offset,pdata,len);
	pair_offset += len;
	check_sum = 0;
	for(i=0;i<len+AIR_HEADER_LEN;i++)
		check_sum += result->pdata[i];
	*pair_offset = check_sum;	
	result->len = (AIR_HEADER_LEN+len+1);
	return AIR_SUCCESS;
}

air_ret_t air_check_pack(uint8_t *pdata,uint16_t len)
{
	uint16_t i;
	uint8_t check_sum = 0;
	
	for(i=0;i<len - 1;i++)
		check_sum += *pdata++;
	if(check_sum == *pdata)
		return AIR_SUCCESS;
	return AIR_FAIL;
}

air_ret_t air_data_parser(uint8_t *pdata,uint16_t len,air_result_t *result)
{
	air_header_t *p_air_header;	
	
	if(air_check_pack(pdata,len))
		return AIR_FAIL;
	
	p_air_header = (air_header_t *)pdata;
	if(p_air_header->tag != AIR_TAG)
		return AIR_FAIL;
	
	if(p_air_header->len > len)
		return AIR_FAIL;
	
	result->cmd = p_air_header->cmd;
	result->pdata = &pdata[AIR_HEADER_LEN];	
	result->len = p_air_header->len;
	return AIR_SUCCESS;
}

void air_show_log(uint8_t *pdata,uint16_t len)
{
	uint16_t i;
	
	for(i=0;i<len;i++)
	{		
		printf("%02X ",pdata[i]);
		if((i+1)%16 == 0)
			printf("\n");
	}
	printf("\n");
}
