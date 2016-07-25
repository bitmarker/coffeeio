#include "CppUTest/TestHarness.h"
#include <cstdio>

extern "C"
{
	#include "ciostack/ciostack.h"  
}

TEST_GROUP(ciostack)
{
  // global vars
 
  void setup()
  {
    
  }

  void teardown() 
  {
    
  }
};

TEST(ciostack, buffer_object)
{
	unsigned char offset = 0;
	char u8 = 0;
	int u16;
	long u32;
	float f32;
	unsigned char buffer[32];
    CIO_BUFFER_OBJECT bufferObject;
    CIO_Buffer_Init(&bufferObject, buffer, sizeof(buffer));

	CHECK_EQUAL(buffer, bufferObject.buffer);
	CHECK_EQUAL(sizeof(buffer), bufferObject.size);

	CIO_Buffer_SerializeChar(&bufferObject, 0xAB);
	CHECK_EQUAL(1, bufferObject.offset);
	CHECK_EQUAL(0xAB, buffer[offset++]);

	CIO_Buffer_SerializeChar(&bufferObject, 0xCD);
	CHECK_EQUAL(2, bufferObject.offset);
	CHECK_EQUAL(0xAB, buffer[0]);
	CHECK_EQUAL(0xCD, buffer[offset++]);

	CIO_Buffer_SerializeInt(&bufferObject, 0xEF75);
	CHECK_EQUAL(0xEF, buffer[offset++]);
	CHECK_EQUAL(0x75, buffer[offset++]);

	CIO_Buffer_SerializeLong(&bufferObject, 0x12345678);
	CHECK_EQUAL(0x12, buffer[offset++]);
	CHECK_EQUAL(0x34, buffer[offset++]);
	CHECK_EQUAL(0x56, buffer[offset++]);
	CHECK_EQUAL(0x78, buffer[offset++]);

	CIO_Buffer_SerializeFloat(&bufferObject, 12.3456);
	CHECK_EQUAL(0x41, buffer[offset++]);
	CHECK_EQUAL(0x45, buffer[offset++]);
	CHECK_EQUAL(0x87, buffer[offset++]);
	CHECK_EQUAL(0x94, buffer[offset++]);

	// Check the offset
	CHECK_EQUAL(offset, bufferObject.offset);

	CIO_Buffer_Reset(&bufferObject);
	CHECK_EQUAL(0, bufferObject.offset);

	CIO_Buffer_DeserializeChar(&bufferObject, &u8);
	CHECK_EQUAL(0xAB, (unsigned char)u8);

	CIO_Buffer_DeserializeChar(&bufferObject, &u8);
	CHECK_EQUAL(0xCD, (unsigned char)u8);

	CIO_Buffer_DeserializeInt(&bufferObject, &u16);
	CHECK_EQUAL(0xEF75, (unsigned int)u16);

	CIO_Buffer_DeserializeLong(&bufferObject, &u32);
	CHECK_EQUAL(0x12345678, (unsigned int)u32);

	CIO_Buffer_DeserializeFloat(&bufferObject, &f32);
	DOUBLES_EQUAL(12.3456, f32, 0.00001);
	
	CHECK_EQUAL(offset, bufferObject.offset);
}

TEST(ciostack, init_frame_init)
{
	CIO_FRAME frameIO;
	COFFEEIO_VARIANT fieldsIO[5];

	CIO_FrameInit(&frameIO, fieldsIO, sizeof(fieldsIO) / sizeof(fieldsIO[0]));

	CHECK_EQUAL(5, frameIO.fieldsCount);
	CHECK_EQUAL(fieldsIO, frameIO.fields);
}

TEST(ciostack, frame_to_buffer)
{
	unsigned char buffer[32];
    CIO_BUFFER_OBJECT bufferObject;
	CIO_FRAME frameIO;
	COFFEEIO_VARIANT fieldsIO[5];

    CIO_Buffer_Init(&bufferObject, buffer, sizeof(buffer));
	CIO_FrameInit(&frameIO, fieldsIO, sizeof(fieldsIO) / sizeof(fieldsIO[0]));

	frameIO.header.flags.byte = 0x12;

	frameIO.fields[0].type = variant_u32;
	frameIO.fields[0].value.u32 = 123;

	frameIO.fields[1].type = variant_u32;
	frameIO.fields[1].value.u32 = 987654321;

	frameIO.fields[2].type = variant_u16;
	frameIO.fields[2].value.u16 = 1234;

	frameIO.fields[3].type = variant_f32;
	frameIO.fields[3].value.f32 = 12.532;

	frameIO.fields[4].type = variant_f32;
	frameIO.fields[4].value.f32 = 15.5;

	CIO_FrameToBuffer(&frameIO, &bufferObject);
}
