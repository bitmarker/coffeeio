#include <stdio.h>
#include "ciostack.h"

void CIO_Buffer_Init(CIO_BUFFER_OBJECT *self, unsigned char *buffer, unsigned char len)
{
	self->buffer = buffer;
    self->size = len;
	self->offset = 0;
}

void CIO_Buffer_Reset(CIO_BUFFER_OBJECT *self)
{
    self->offset = 0;
}

void CIO_Buffer_PutByte(CIO_BUFFER_OBJECT *self, unsigned char dataByte)
{
	if(self->offset < self->size)
	{
		self->buffer[self->offset] = dataByte;
		self->offset += 1;
	}
}

void CIO_CheckForFrame(CIO_BUFFER_OBJECT *self)
{
	if(self->offset > 2)
	{
		unsigned char start_byte = self->buffer[0];

		if(start_byte != 0xAA)
		{
			return;
		}

		unsigned char size = self->buffer[1];
		
		if(self->offset < size)
		{
			return;
		}

		if(self->offset >= size)
		{
			printf("-> {%d}\n", self->offset);

			self->offset = 0;
		}

		printf("{%d}\n", self->offset);
	}
}



CIO_RESULT CIO_Buffer_SerializeChar(CIO_BUFFER_OBJECT *self, char data)
{
    if(self->offset > (self->size - 1)) {
        return CIO_RESULT_ERROR;
    }

    self->buffer[self->offset] = data;

    self->offset += 1;

    return CIO_RESULT_GOOD;
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
			CIO_Buffer_SerializeChar(bufferObject, (signed char)variant->value.u8);
			break;
		case variant_u16:
		case variant_s16:
			CIO_Buffer_SerializeInt(bufferObject, (signed int)variant->value.u16);
			break;
		case variant_u32:
		case variant_s32:
			CIO_Buffer_SerializeLong(bufferObject, (signed long)variant->value.u32);
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

void CIO_FrameInit(CIO_FRAME *frame, COFFEEIO_VARIANT *fields, unsigned int fieldsCount)
{
	unsigned int i;

	frame->header.start_byte = CIO_START_BYTE;

	frame->fieldsCount = fieldsCount;
	
	frame->fields = fields;

	for(i = 0; i < frame->fieldsCount; i++)
	{
		frame->fields[i].type = variant_none;
	}
}

void CIO_Buffer_CalculateCRC(CIO_BUFFER_OBJECT *self, unsigned char *crc)
{
	*crc = crc8(self->buffer, self->offset);
}

unsigned int CIO_FrameSize(CIO_FRAME *frame)
{
	unsigned int i;
	unsigned int size = sizeof(frame->header);
	
	for(i = 0; i < frame->fieldsCount; i++)
	{
		size += CIO_VariantSize(&frame->fields[i]);
	}

	return size;
}

unsigned int CIO_FrameToBuffer(CIO_FRAME *frame, CIO_BUFFER_OBJECT *bufferObject) 
{
	unsigned int i;

	frame->header.size = CIO_FrameSize(frame);

	CIO_Buffer_SerializeChar(bufferObject, frame->header.start_byte);
	CIO_Buffer_SerializeChar(bufferObject, frame->header.size);
	CIO_Buffer_SerializeChar(bufferObject, frame->header.flags.byte);

	for(i = 0; i < frame->fieldsCount; i++)
	{
		CIO_Buffer_SerializeVariant(bufferObject, &frame->fields[i]);
	}

	CIO_Buffer_CalculateCRC(bufferObject, &frame->header.crc);

	CIO_Buffer_SerializeChar(bufferObject, frame->header.crc);

	return 0;
}

CIO_RESULT CIO_FrameFromBuffer(CIO_FRAME *frame, CIO_BUFFER_OBJECT *bufferObject) 
{
	unsigned int i;
	unsigned char crc, crc_temp;
	CIO_RESULT result;

	result = CIO_Buffer_DeserializeChar(bufferObject, &frame->header.start_byte);

	if(result != CIO_RESULT_GOOD)
	{
		return result;
	}

	// Get the size of the whole frame
	result = CIO_Buffer_DeserializeChar(bufferObject, &frame->header.size);

	if(result != CIO_RESULT_GOOD)
	{
		return result;
	}

	result = CIO_Buffer_DeserializeChar(bufferObject, &frame->header.flags.byte);

	if(result != CIO_RESULT_GOOD)
	{
		return result;
	}

	for(i = 0; i < frame->fieldsCount; i++)
	{
		result = CIO_Buffer_DeserializeVariant(bufferObject, &frame->fields[i]);

		if(result != CIO_RESULT_GOOD)
		{
			return result;
		}
	}

	CIO_Buffer_CalculateCRC(bufferObject, &crc_temp);

	result = CIO_Buffer_DeserializeChar(bufferObject, &crc);

	if(result != CIO_RESULT_GOOD)
	{
		return result;
	}

	if(crc != crc_temp)
	{
		return CIO_RESULT_ERROR;
	}

	return CIO_RESULT_GOOD;
}

void print_bufferObject(CIO_BUFFER_OBJECT *self)
{
    unsigned int i;

    printf("Buffer size: %d\n", self->offset);

    for(i = 0; i < self->offset; i++) {
        printf("[%2.2d] %2.2X (%d)\n", i, self->buffer[i], self->buffer[i]);
    }
}

/* Copyright (c) 2013 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Return CRC-8 of the data, using x^8 + x^2 + x + 1 polynomial.  A table-based
 * algorithm would be faster, but for only a few bytes it isn't worth the code
 * size. */
unsigned char crc8(const void *vptr, int len)
{
	const unsigned char *data = vptr;
	unsigned int crc = 0;
	int i, j;

	for (j = len; j; j--, data++) {
		crc ^= (*data << 8);
		for(i = 8; i; i--) {
			if (crc & 0x8000)
				crc ^= (0x1070 << 3);
			crc <<= 1;
		}
	}

	return (unsigned char)(crc >> 8);
}
