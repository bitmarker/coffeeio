#ifndef CIOSTACK_H
#define CIOSTACK_H


typedef char CIO_RESULT;

#define CIO_RESULT_GOOD     0
#define CIO_RESULT_ERROR    -1

typedef struct __attribute__((__packed__)) coffeeio_frame_header
{
	unsigned char start_byte;
    unsigned char size;
    unsigned char crc;

    union _flags
    {
        struct
        {
            unsigned char watchdog:1;
            unsigned char type:1;
            unsigned char reserverd_2:1;
            unsigned char reserverd_3:1;
            unsigned char reserverd_4:1;
            unsigned char reserverd_5:1;
            unsigned char reserverd_6:1;
            unsigned char reserverd_7:1;
        };

        unsigned char byte;
    } flags;
} CIO_FRAME_HEADER;

typedef struct cofeeio_buffer_object
{
    unsigned char *buffer;
    unsigned int size;
    unsigned int offset;
} CIO_BUFFER_OBJECT;


typedef enum _coffeeio_variant_type
{
	variant_none = 0,
	variant_u8,
	variant_s8,
	variant_u16,
	variant_s16,
	variant_u32,
	variant_s32,
	variant_f32
} COFFEEIO_VARIANT_TYPE;

typedef struct _coffeeio_variant
{
	union _value {
		unsigned char u8;
		char s8;
		unsigned int u16;
		int s16;
		unsigned long u32;
		long s32;
		float f32;
	} value;
	COFFEEIO_VARIANT_TYPE type;
} COFFEEIO_VARIANT;

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
void CIO_Buffer_SerializeInt(CIO_BUFFER_OBJECT *self, int data);
void CIO_Buffer_SerializeLong(CIO_BUFFER_OBJECT *self, long data);
void CIO_Buffer_SerializeFloat(CIO_BUFFER_OBJECT *self, float data);
CIO_RESULT CIO_Buffer_DeserializeLong(CIO_BUFFER_OBJECT *self, long *data);
CIO_RESULT CIO_Buffer_DeserializeChar(CIO_BUFFER_OBJECT *self, char *data);



#endif
