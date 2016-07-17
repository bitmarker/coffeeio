#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ciostack.h"


void print_bufferObject(CIO_BUFFER_OBJECT *self)
{
    unsigned int i;

    printf("Buffer size: %d\n", self->offset);

    for(i = 0; i < self->offset; i++) {
        printf("%d: %X (%d)\n", i, self->buffer[i], self->buffer[i]);
    }
}


#define DEFINE_CIO_FRAME(type, fields_count) \
	typedef struct cofeeio_frame_##type \
	{ \
	    CIO_FRAME_HEADER header; \
		COFFEEIO_VARIANT fields[fields_count]; \
	} CIO_FRAME_##type;


DEFINE_CIO_FRAME(IO, 5);
DEFINE_CIO_FRAME(HOST, 2);


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

unsigned int CIO_IOFrameToBuffer(CIO_FRAME_IO *frame, CIO_BUFFER_OBJECT *bufferObject) 
{
	unsigned int i;
	unsigned int fieldsCount = sizeof(frame->fields) / sizeof(frame->fields[0]);

	frame->header.start_byte = 0xAA;
	frame->header.size = sizeof(frame->header);

	for(i = 0; i < fieldsCount; i++)
	{
		frame->header.size += CIO_VariantSize(&frame->fields[i]);
	}

	CIO_Buffer_SerializeChar(bufferObject, frame->header.start_byte);
	CIO_Buffer_SerializeChar(bufferObject, frame->header.size);
	CIO_Buffer_SerializeChar(bufferObject, frame->header.flags.byte);

	for(i = 0; i < fieldsCount; i++)
	{
		CIO_Buffer_SerializeVariant(bufferObject, &frame->fields[i]);
	}

	CIO_Buffer_SerializeChar(bufferObject, frame->header.crc);

	return 0;
}

int main() {
	// Host = Linux PC
	// IO = Arduino Board

	unsigned char buffer[20];
    CIO_BUFFER_OBJECT bufferObject;


    CIO_Buffer_Init(&bufferObject);

    bufferObject.buffer = buffer;
    bufferObject.size = sizeof(buffer);


	
	CIO_FRAME_IO frameIO;

	frameIO.header.flags.byte = 0xFF;


	frameIO.fields[0].type = variant_none;
	frameIO.fields[1].type = variant_none;
	frameIO.fields[2].type = variant_none;
	frameIO.fields[3].type = variant_none;
	frameIO.fields[4].type = variant_none;


	frameIO.fields[0].type = variant_u32;
	frameIO.fields[0].value.u32 = 0xABCDEF12;

	frameIO.fields[1].type = variant_u32;
	frameIO.fields[1].value.u32 = 0xABCDEF12;

	frameIO.fields[2].type = variant_u16;
	frameIO.fields[2].value.u16 = 0x1234;

	frameIO.fields[3].type = variant_f32;
	frameIO.fields[3].value.f32 = 12.5;

	frameIO.fields[4].type = variant_f32;
	frameIO.fields[4].value.f32 = 0.5;


/*
	frameIO.payload.somedata = 0xABCDEF;
	frameIO.payload.temperature_1 = 20.5;
	frameIO.payload.temperature_2 = 25.2;
*/
	
	CIO_IOFrameToBuffer(&frameIO, &bufferObject);

	print_bufferObject(&bufferObject);


	return 0;
}



#ifdef comment


static unsigned int cofeeio_frame_payload_size = 0;

typedef struct __attribute__((__packed__)) cofeeio_frame_payload
{
    unsigned char somedata;
    unsigned int rpm;
    float temperature;
} CIO_FRAME_PAYLOAD;



void CIO_Buffer_CalculateCRC(CIO_BUFFER_OBJECT *self, unsigned char *crc)
{

}


int main()
{
    unsigned char buffer[20];
    CIO_BUFFER_OBJECT bufferObject;

    CIO_Buffer_Init(&bufferObject);

    bufferObject.buffer = buffer;
    bufferObject.size = sizeof(buffer);




    printf("sizeof(CIO_FRAME): %d\n", sizeof(CIO_FRAME));


    CIO_FRAME frameOut;


    frameOut.start_byte = 0xAA;

    frameOut.size = sizeof(CIO_FRAME);
    frameOut.crc = 0xFF;

    frameOut.flags.watchdog = 1;
    frameOut.flags.type = 1;

    frameOut.payload.rpm = 0xABCDEF12;
    frameOut.payload.somedata = 0xBB;
    frameOut.payload.temperature = 15.4;


    // Write the start byte
    CIO_Buffer_SerializeChar(&bufferObject, frameOut.start_byte);

    // Put the size to the buffer and add one byte to the size
    CIO_Buffer_SerializeChar(&bufferObject, frameOut.size);

    // Put the flags
    CIO_Buffer_SerializeChar(&bufferObject, frameOut.flags.byte);

    // Payload - START
    CIO_Buffer_SerializeChar(&bufferObject, frameOut.payload.somedata);
    CIO_Buffer_SerializeLong(&bufferObject, frameOut.payload.rpm);
    CIO_Buffer_SerializeFloat(&bufferObject, frameOut.payload.temperature);
    // Payload - END

    // Calculate the CRC and write it to the buffer
    CIO_Buffer_CalculateCRC(&bufferObject, &frameOut.crc);
    CIO_Buffer_SerializeChar(&bufferObject, frameOut.crc);


    print_bufferObject(&bufferObject);

    /*
    CIO_Buffer_SerializeLong(&bufferObject, 123456);
    CIO_Buffer_SerializeLong(&bufferObject, 789);
    CIO_Buffer_SerializeChar(&bufferObject, 15);

    printf("Buffer Size: %d Bytes\n", bufferObject.offset);




    CIO_Buffer_Reset(&bufferObject);



    long long_val;
    char char_val;

    CIO_Buffer_DeserializeLong(&bufferObject, &long_val);
    printf("%ld\n", long_val);

    CIO_Buffer_DeserializeLong(&bufferObject, &long_val);
    printf("%ld\n", long_val);

    if(CIO_Buffer_DeserializeChar(&bufferObject, &char_val) == CIO_RESULT_GOOD)
    {
       printf("%d\n", char_val);
    }

*/

    return 0;
}

#endif
