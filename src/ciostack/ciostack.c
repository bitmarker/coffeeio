#include <stdio.h>
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


CIO_RESULT CIO_Buffer_DeserializeFloat(CIO_BUFFER_OBJECT *self, float *data)
{
    CIO_BUFFER_CHUNK chunk;

    if(self->offset > (self->size - 4)) {
        return CIO_RESULT_ERROR;
    }

    chunk.bytes = self->buffer[self->offset + 0] << 24 |
            	self->buffer[self->offset + 1] << 16 |
            	self->buffer[self->offset + 2] << 8 |
            	self->buffer[self->offset + 3];

	*data = chunk.float_var;

    self->offset += 4;

    return CIO_RESULT_GOOD;
}


CIO_RESULT CIO_Buffer_DeserializeInt(CIO_BUFFER_OBJECT *self, int *data)
{
    if(self->offset > (self->size - 2)) {
        return CIO_RESULT_ERROR;
    }

    *data = self->buffer[self->offset + 0] << 8 |
            self->buffer[self->offset + 1];

    self->offset += 2;

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



unsigned char CIO_VariantSize(COFFEEIO_VARIANT *variant)
{
	switch(variant->type)
	{
		case variant_u8:
		case variant_s8:
			return 1;
		case variant_u16:
		case variant_s16:
			return 2;
		case variant_u32:
		case variant_s32:
		case variant_f32:
			return 4;
		default:
			return 0;
	}
}

void CIO_Buffer_SerializeVariant(CIO_BUFFER_OBJECT *bufferObject, COFFEEIO_VARIANT *variant)
{
	switch(variant->type)
	{
		case variant_u8:
		case variant_s8:
			CIO_Buffer_SerializeChar(bufferObject, variant->value.u8);
			break;
		case variant_u16:
		case variant_s16:
			CIO_Buffer_SerializeInt(bufferObject, variant->value.u16);
			break;
		case variant_u32:
		case variant_s32:
			CIO_Buffer_SerializeLong(bufferObject, variant->value.u32);
			break;
		case variant_f32:
			CIO_Buffer_SerializeFloat(bufferObject, variant->value.f32);
			break;
	}
}

CIO_RESULT CIO_Buffer_DeserializeVariant(CIO_BUFFER_OBJECT *bufferObject, COFFEEIO_VARIANT *variant)
{
	switch(variant->type)
	{
		case variant_u8:
		case variant_s8:
			return CIO_Buffer_DeserializeChar(bufferObject, &variant->value.u8);
		case variant_u16:
		case variant_s16:
			return CIO_Buffer_DeserializeInt(bufferObject, &variant->value.u16);
		case variant_u32:
		case variant_s32:
			return CIO_Buffer_DeserializeLong(bufferObject, &variant->value.u32);
		case variant_f32:
			return CIO_Buffer_DeserializeFloat(bufferObject, &variant->value.f32);
	}
}

void CIO_VariantPrint(COFFEEIO_VARIANT *variant)
{
	switch(variant->type)
	{
		case variant_u8:
			printf("<u8> %u\n", variant->value.u8);
			break;
		case variant_s8:
			printf("<s8> %d\n", variant->value.s8);
			break;
		case variant_u16:
			printf("<u16> %u\n", variant->value.u16);
			break;
		case variant_s16:
			printf("<s16> %d\n", variant->value.s16);
			break;
		case variant_u32:
			printf("<u32> %lu\n", variant->value.u32);
			break;
		case variant_s32:
			printf("<s32> %ld\n", variant->value.s32);
			break;
		case variant_f32:
			printf("<f32> %f\n", variant->value.f32);
			break;
		default:
			printf("<unknown>\n");
	}
}

