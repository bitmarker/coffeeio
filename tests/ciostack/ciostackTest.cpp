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

TEST(ciostack, init_buffer_object)
{
	unsigned char buffer[32];
    CIO_BUFFER_OBJECT bufferObject;
    CIO_Buffer_Init(&bufferObject, buffer, sizeof(buffer));

	CHECK_EQUAL(buffer, bufferObject.buffer);
	CHECK_EQUAL(sizeof(buffer), bufferObject.size);
}

TEST(ciostack, init_frame_init)
{
	CIO_FRAME frameIO;
	COFFEEIO_VARIANT fieldsIO[5];

	CIO_FrameInit(&frameIO, fieldsIO, sizeof(fieldsIO) / sizeof(fieldsIO[0]));

	CHECK_EQUAL(5, frameIO.fieldsCount);
	CHECK_EQUAL(fieldsIO, frameIO.fields);
}

TEST(ciostack, xy)
{
	CIO_FRAME frameIO;
	COFFEEIO_VARIANT fieldsIO[5];

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

	
}
