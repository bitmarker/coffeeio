#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ciostack.h"




int main(int argc, char **argv) {
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
	
	if(argc >= 2)
		frameIO.fields[0].value.u32 = atoi(argv[1]);
	else
		frameIO.fields[0].value.u32 = 0;

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
