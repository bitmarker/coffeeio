#ifndef CIOSTACK_H
#define CIOSTACK_H


typedef char CIO_RESULT;

#define CIO_RESULT_GOOD     0
#define CIO_RESULT_ERROR    -1



typedef struct cofeeio_buffer_object
{
    unsigned char *buffer;
    unsigned int size;
    unsigned int offset;
} CIO_BUFFER_OBJECT;



typedef union cofeeio_buffer_chunk
{
    struct {
        float float_var;
    };
    unsigned long bytes;
} CIO_BUFFER_CHUNK;

void CIO_Buffer_Init(CIO_BUFFER_OBJECT *self);
void CIO_Buffer_Reset(CIO_BUFFER_OBJECT *self);
void CIO_Buffer_SerializeChar(CIO_BUFFER_OBJECT *self, char data);
void CIO_Buffer_SerializeLong(CIO_BUFFER_OBJECT *self, long data);
void CIO_Buffer_SerializeFloat(CIO_BUFFER_OBJECT *self, float data);
CIO_RESULT CIO_Buffer_DeserializeLong(CIO_BUFFER_OBJECT *self, long *data);
CIO_RESULT CIO_Buffer_DeserializeChar(CIO_BUFFER_OBJECT *self, char *data);



#endif
