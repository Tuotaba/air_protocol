#ifndef _FIFO_BUFFER_H
#define _FIFO_BUFFER_H

#define buffer_type_t	unsigned char

typedef struct
{
    buffer_type_t *m_buffer;
    int start_index;
    int end_index;
	int max_len;
} buffer_list_t;

void buffer_init(buffer_list_t *list,buffer_type_t *pBuf,int len);
int buffer_find(buffer_list_t *list,const buffer_type_t *match,int len);
int buffer_append(buffer_list_t *list,const buffer_type_t *data,int len);
int buffer_pop(buffer_list_t *list,buffer_type_t *data,int len);
int buffer_compare(buffer_list_t *list,int start,const buffer_type_t *target,int len);
buffer_type_t buffer_get(buffer_list_t *list,int index);
int get_data_length(buffer_list_t *list);
void buffer_print(buffer_list_t *list);
#endif
