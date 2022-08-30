#include "air_protocol.h"
#include "fifo_buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_BUFFER_LEN	512
static uint8_t pUartData[MAX_BUFFER_LEN];
static buffer_list_t UartBuf;

static uint8_t air_buffer[MAX_BUFFER_LEN];

typedef struct
{	
	uint32_t addr;
	uint32_t len;
	uint8_t data[256];
}_file_data_t;

#define CMD_FILE_BEGIN	0x10
#define CMD_FILE_DATA	0x11
#define CMD_FILE_END	0x12

void send_uart(uint8_t cmd,uint8_t *buf,uint16_t len)
{
	air_result_t result;
	air_ret_t ret;
	
	ret = air_alloc_pack(cmd,buf,len,&result);
	if(ret == AIR_FAIL)
		return;	
	buffer_append(&UartBuf,result.pdata,result.len);	
	free(result.pdata);
}

static void command_handler(uint8_t cmd,uint8_t *buf,uint16_t len)
{	
	_file_data_t file_data;
	static uint32_t require_addr = 0;
	
	switch(cmd)
	{
		case CMD_FILE_BEGIN:		
			require_addr = 0;
		break;
		case CMD_FILE_DATA:			
			memcpy(&file_data,buf,len);
			printf("%08X :\n",file_data.addr);
			if(file_data.addr == require_addr){
				air_show_log(file_data.data,file_data.len);
				require_addr += file_data.len;
			}
			else{
				printf("please resend addr %08X\n",require_addr);
			}
		break;
		case CMD_FILE_END:
			printf("file is done\n");
			break;
		default:break;
	}
}

void uart_data_parser(void)
{
	air_result_t result;
	air_ret_t ret;
	air_header_t air_header;
	uint8_t *pdata;
	int i,len;
	
	air_header.tag = AIR_TAG;
	do{
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
						command_handler(result.cmd,result.pdata,result.len);
					}
					continue;
				}
			}			
		}
		break;
	}while(1);
}

int main()
{		
	int len;
	FILE *fp = NULL;
	
	_file_data_t file_data;
	
	buffer_init(&UartBuf,pUartData,MAX_BUFFER_LEN);
	
	file_data.addr = 0;
		
	fp = fopen("./test.bin","r");
	if(fp == NULL)
	{
		printf("not found test.bin\n");
		return 0;
	}	
	send_uart(CMD_FILE_BEGIN,NULL,0);	
	while(1){		
		len = fread(file_data.data,1,256,fp);
		if(len <= 0)
			break;
		file_data.len = len;
		send_uart(CMD_FILE_DATA,(uint8_t*)&file_data,file_data.len+8);
		uart_data_parser();
		file_data.addr += len;
	}
	send_uart(CMD_FILE_END,NULL,0);
	uart_data_parser();
	fclose(fp);
	return 0;
}
