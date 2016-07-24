#include "CppUTest/TestHarness.h"

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
	unsigned char buffer[32];
    CIO_BUFFER_OBJECT bufferObject;
    CIO_Buffer_Init(&bufferObject, buffer, sizeof(buffer));

	CHECK_EQUAL(buffer, bufferObject.buffer);
	CHECK_EQUAL(sizeof(buffer), bufferObject.size);

	CIO_Buffer_SerializeChar(&bufferObject, 0xAB);
	CHECK_EQUAL(1, bufferObject.offset);
	CHECK_EQUAL(0xAB, buffer[0]);

	CIO_Buffer_SerializeChar(&bufferObject, 0xCD);
	CHECK_EQUAL(2, bufferObject.offset);
	CHECK_EQUAL(0xAB, buffer[0]);
	CHECK_EQUAL(0xCD, buffer[1]);

	// TODO: Test different data types

	CIO_Buffer_Reset(&bufferObject);
	CHECK_EQUAL(0, bufferObject.offset);
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
