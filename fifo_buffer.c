#include <string.h>
#include <stdio.h>
#include "fifo_buffer.h"

void buffer_init(buffer_list_t *list,buffer_type_t *pBuf,int len)
{
	list->m_buffer = pBuf;
	list->max_len = len;
	list->start_index = 0;
	list->end_index = 0;
}

static int get_left_space(buffer_list_t *list)
{	
	if(list->start_index <= list->end_index)
		return list->max_len - (list->end_index - list->start_index + 1);
	
	return list->start_index - list->end_index - 1;
}

int get_data_length(buffer_list_t *list)
{	
	if(list->start_index <= list->end_index)
		return list->end_index - list->start_index;
	
	return list->max_len - list->start_index + list->end_index;
}

int buffer_find(buffer_list_t *list,const buffer_type_t *match,int len)
{
	int i,j,k;
	int pos;
	int length = get_data_length(list);
	
	if(length ==0)
		return -1;
	if(len > length)
		return -1;
		
	for(i=0;i<length;i++)
	{
		for(j = 0,k = i;j<len && k<length;j++,k++)
		{
			pos = list->start_index + k;
			if(pos >= list->max_len)
				pos -= list->max_len;
			if(match[j] != list->m_buffer[pos])
				break;
		}
		if(j == len)
			return i;
	}
	return -1;
}

int buffer_append(buffer_list_t *list,const buffer_type_t *data,int len)
{
	int i;
	int length = get_left_space(list);
	
	if(length < len)
	{
		buffer_pop(list,NULL,(len - length + 1));
	}

	length = get_left_space(list);
	if(len > length)
		len = length;
	for(i=0;i<len;i++)
	{
		list->m_buffer[list->end_index++] = data[i];
		if(list->end_index >= list->max_len)
			list->end_index = 0;
	}
	return len;
}

int buffer_pop(buffer_list_t *list,buffer_type_t *data,int len)
{
	int i;
	int length = get_data_length(list);
	
	if(length ==0)
		return 0;
	if(len > length)
		len = length;
	for(i=0;i<len;i++)
	{
		if(data!=NULL)
			data[i] = list->m_buffer[list->start_index];
		list->m_buffer[list->start_index] = '\0';
		list->start_index++;
		if(list->start_index >= list->max_len)
			list->start_index = 0;
	}
	return len;
}

int buffer_compare(buffer_list_t *list,int start,const buffer_type_t *target,int len)
{
	int i,j = list->start_index;
	
	if(len > (get_data_length(list) - start))
		return 0;
	
	for(i=0;i<len;i++)
	{
		j = list->start_index + start+i;
		if(j >= list->max_len)
			j -= list->max_len;
		if(list->m_buffer[j] != target[i])
			return 0;
	}
	
	list->start_index = j+1;
	if(list->start_index >= list->max_len)
		list->start_index = 0;
	return 1;
}

buffer_type_t buffer_get(buffer_list_t *list,int index)
{
	int i;
	int len = get_data_length(list);
	if(index >= len)
		return 0;
	i = list->start_index + index;
	if(i >= list->max_len)
		i -= list->max_len;
	return list->m_buffer[i];
}

void buffer_print(buffer_list_t *list)
{
	int i,j;
	
	int len = get_data_length(list);
	for(i=0;i<len;i++)
	{
		j = list->start_index + i;
		if(j >= list->max_len)
			j -= list->max_len;
		printf("%c",list->m_buffer[j]);	
	}		
	printf("\n");
}