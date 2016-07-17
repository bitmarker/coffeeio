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



typedef struct cofeeio_frame
{
    CIO_FRAME_HEADER header;
	COFFEEIO_VARIANT *fields;
	unsigned char fieldsCount;
} CIO_FRAME;

void CIO_FrameInit(CIO_FRAME *frame, COFFEEIO_VARIANT *fields, unsigned int fieldsCount)
{
	unsigned int i;

	frame->header.start_byte = 0xAA;

	frame->fieldsCount = fieldsCount;
	
	frame->fields = fields;

	for(i = 0; i < frame->fieldsCount; i++)
	{
		frame->fields[i].type = variant_none;
	}
}

unsigned int CIO_FrameToBuffer(CIO_FRAME *frame, CIO_BUFFER_OBJECT *bufferObject) 
{
	unsigned int i;

	frame->header.size = sizeof(frame->header);

	for(i = 0; i < frame->fieldsCount; i++)
	{
		frame->header.size += CIO_VariantSize(&frame->fields[i]);
	}

	CIO_Buffer_SerializeChar(bufferObject, frame->header.start_byte);
	CIO_Buffer_SerializeChar(bufferObject, frame->header.size);
	CIO_Buffer_SerializeChar(bufferObject, frame->header.flags.byte);

	for(i = 0; i < frame->fieldsCount; i++)
	{
		CIO_Buffer_SerializeVariant(bufferObject, &frame->fields[i]);
	}

	// TODO: Calculate CRC

	CIO_Buffer_SerializeChar(bufferObject, frame->header.crc);

	return 0;
}

CIO_RESULT CIO_FrameFromBuffer(CIO_FRAME *frame, CIO_BUFFER_OBJECT *bufferObject) 
{
	unsigned int i;
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

	result = CIO_Buffer_DeserializeChar(bufferObject, &frame->header.crc);

	if(result != CIO_RESULT_GOOD)
	{
		return result;
	}

	return CIO_RESULT_GOOD;
}

int main() {
	unsigned char buffer[32];
    CIO_BUFFER_OBJECT bufferObject;

	COFFEEIO_VARIANT fieldsIO[5];

    CIO_Buffer_Init(&bufferObject);
    bufferObject.buffer = buffer;
    bufferObject.size = sizeof(buffer);
	

	CIO_FRAME frameIO;


	
	CIO_FrameInit(&frameIO, fieldsIO, sizeof(fieldsIO) / sizeof(fieldsIO[0]));


	frameIO.header.flags.byte = 0x12;

	frameIO.fields[0].type = variant_u32;
	frameIO.fields[0].value.u32 = 1234567;

	frameIO.fields[1].type = variant_u32;
	frameIO.fields[1].value.u32 = 987654321;

	frameIO.fields[2].type = variant_u16;
	frameIO.fields[2].value.u16 = 1234;

	frameIO.fields[3].type = variant_f32;
	frameIO.fields[3].value.f32 = 12.532;

	frameIO.fields[4].type = variant_f32;
	frameIO.fields[4].value.f32 = 15.5;

	

	CIO_FrameToBuffer(&frameIO, &bufferObject);

	print_bufferObject(&bufferObject);

	CIO_Buffer_Reset(&bufferObject);


	{
		unsigned int i;
		COFFEEIO_VARIANT fieldsIO_temp[5];
		CIO_FRAME frameIO_temp;
		CIO_RESULT result;

		CIO_FrameInit(&frameIO_temp, fieldsIO_temp, sizeof(fieldsIO_temp) / sizeof(fieldsIO_temp[0]));

		frameIO_temp.fields[0].type = variant_u32;
		frameIO_temp.fields[1].type = variant_u32;
		frameIO_temp.fields[2].type = variant_u16;
		frameIO_temp.fields[3].type = variant_f32;
		frameIO_temp.fields[4].type = variant_f32;

		result = CIO_FrameFromBuffer(&frameIO_temp, &bufferObject);

		if(result != CIO_RESULT_GOOD)
		{
			printf("result != CIO_RESULT_GOOD\n");
			return -1;
		}
		
		for(i = 0; i < 5; i++)
		{
			CIO_VariantPrint(&fieldsIO_temp[i]);
		}
	}




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
