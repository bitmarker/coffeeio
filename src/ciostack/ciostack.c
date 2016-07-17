#include "ciostack.h"

void CIO_Buffer_Init(CIO_BUFFER_OBJECT *self)
{
    self->buffer = 0;
    self->size = 0;
    self->offset = 0;
}

void CIO_Buffer_Reset(CIO_BUFFER_OBJECT *self)
{
    self->offset = 0;
}

void CIO_Buffer_SerializeChar(CIO_BUFFER_OBJECT *self, char data)
{
    self->buffer[self->offset] = data;
    self->offset += 1;
}

void CIO_Buffer_SerializeLong(CIO_BUFFER_OBJECT *self, long data)
{
    self->buffer[self->offset] = data >> 24;
    self->buffer[self->offset + 1] = data >> 16;
    self->buffer[self->offset + 2] = data >> 8;
    self->buffer[self->offset + 3] = data;
    self->offset += 4;
}

void CIO_Buffer_SerializeInt(CIO_BUFFER_OBJECT *self, int data)
{
    self->buffer[self->offset] = data >> 8;
    self->buffer[self->offset + 1] = data;
    self->offset += 2;
}

void CIO_Buffer_SerializeFloat(CIO_BUFFER_OBJECT *self, float data)
{
    CIO_BUFFER_CHUNK chunk;

    chunk.float_var = data;

    self->buffer[self->offset] = chunk.bytes >> 24;
    self->buffer[self->offset + 1] = chunk.bytes >> 16;
    self->buffer[self->offset + 2] = chunk.bytes >> 8;
    self->buffer[self->offset + 3] = chunk.bytes;
    self->offset += 4;
}

CIO_RESULT CIO_Buffer_DeserializeLong(CIO_BUFFER_OBJECT *self, long *data)
{
    if(self->offset > (self->size - 4)) {
        return CIO_RESULT_ERROR;
    }

    *data = self->buffer[self->offset + 0] << 24 |
            self->buffer[self->offset + 1] << 16 |
            self->buffer[self->offset + 2] << 8 |
            self->buffer[self->offset + 3];

    self->offset += 4;

    return CIO_RESULT_GOOD;
}

CIO_RESULT CIO_Buffer_DeserializeChar(CIO_BUFFER_OBJECT *self, char *data)
{
    if(self->offset > (self->size - 1)) {
        return CIO_RESULT_ERROR;
    }

    *data = self->buffer[self->offset + 0];

    self->offset += 1;

    return CIO_RESULT_GOOD;
}

