#include "air_protocol.h"
#include "fifo_buffer.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_BUFFER_LEN	256
static uint8_t pUartData[MAX_BUFFER_LEN];
static buffer_list_t UartBuf;

static uint8_t air_buffer[MAX_BUFFER_LEN];

void send_uart(uint8_t cmd,uint8_t *buf,uint16_t len)
{
	air_result_t result;
	air_ret_t ret;
	
	ret = air_alloc_pack(cmd,buf,len,&result);
	if(ret == AIR_FAIL)
		return;
	printf("send:\n");
	air_show_log(result.pdata,result.len);		
	printf("\n");
	free(result.pdata);
}

void uart_data_parser(void)
{
	air_result_t result;
	air_ret_t ret;
	air_header_t air_header;
	uint8_t *pdata;
	int i,len;
	
	air_header.tag = AIR_TAG;
	i = buffer_find(&UartBuf,(uint8_t *)&air_header.tag,sizeof(air_header.tag));
	if(i >= 0)
	{
		if(i>0)
			buffer_pop(&UartBuf,NULL,i);
		if(get_data_length(&UartBuf) > AIR_HEADER_LEN)
		{
			pdata = (uint8_t *)&air_header;
			for(i=0;i<AIR_HEADER_LEN;i++)
				pdata[i] = buffer_get(&UartBuf,i);
			len = (AIR_HEADER_LEN+air_header.len+1);
			if(get_data_length(&UartBuf) >= len)
			{
				buffer_pop(&UartBuf,air_buffer,len);
				ret = air_data_parser(air_buffer,len,&result);
				if(ret == AIR_SUCCESS)
				{
					printf("recv:cmd = %02X,len =%d\n",result.cmd,result.len);
					air_show_log(result.pdata,result.len);
					printf("\n");
				}
			}
		}
	}
}

int main()
{		
	int i,len;
	
	uint8_t buf[] = {0x8D ,0x7C ,0x6B ,0x5A ,0x0A ,0x00 ,0x0F ,0x00 ,0xAA ,0xAA ,0xAA ,0xAA ,0xAA ,0xAA ,0xAA ,0xAA ,0xAA ,0xAA ,0xAA ,0xAA ,0xAA ,0xAA ,0xAA ,0xDD};
	uint8_t data[] = {0xAA ,0xAA ,0xAA ,0xAA ,0xAA ,0xAA ,0xAA ,0xAA ,0xAA ,0xAA ,0xAA ,0xAA ,0xAA ,0xAA ,0xAA};
		
	buffer_init(&UartBuf,pUartData,MAX_BUFFER_LEN);
	
	for(i=0;i<sizeof(buf);i++)
	{
		buffer_append(&UartBuf,&buf[i],1);
		uart_data_parser();
	}
	
	send_uart(0x0A,data,sizeof(data));
	return 0;
}
