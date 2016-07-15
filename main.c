#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char CIO_RESULT;

#define CIO_RESULT_GOOD     0
#define CIO_RESULT_ERROR    -1

typedef struct cofeeio_buffer_object
{
    unsigned char *buffer;
    unsigned int size;
    unsigned int offset;
} CIO_BUFFER_OBJECT;



typedef union cofeeio_buffer_chunk
{
    struct {
        float float_var;
    };
    unsigned long bytes;
} CIO_BUFFER_CHUNK;

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

static unsigned int cofeeio_frame_payload_size = 0;

typedef struct __attribute__((__packed__)) cofeeio_frame_payload
{
    unsigned char somedata;
    unsigned int rpm;
    float temperature;
} CIO_FRAME_PAYLOAD;

typedef struct __attribute__((__packed__)) cofeeio_frame
{
    unsigned char start_byte;
    unsigned char size;
    unsigned char crc;

    CIO_FRAME_PAYLOAD payload;

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

} CIO_FRAME;

void CIO_Buffer_CalculateCRC(CIO_BUFFER_OBJECT *self, unsigned char *crc)
{

}

void print_buffer_object(CIO_BUFFER_OBJECT *self)
{
    unsigned int i;

    printf("Buffer size: %d\n", self->offset);

    for(i = 0; i < self->offset; i++) {
        printf("%d: %X (%d)\n", i, self->buffer[i], self->buffer[i]);
    }
}

int main()
{
    unsigned char buffer[20];
    CIO_BUFFER_OBJECT buffer_object;

    CIO_Buffer_Init(&buffer_object);

    buffer_object.buffer = buffer;
    buffer_object.size = sizeof(buffer);




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
    CIO_Buffer_SerializeChar(&buffer_object, frameOut.start_byte);

    // Put the size to the buffer and add one byte to the size
    CIO_Buffer_SerializeChar(&buffer_object, frameOut.size);

    // Put the flags
    CIO_Buffer_SerializeChar(&buffer_object, frameOut.flags.byte);

    // Payload - START
    CIO_Buffer_SerializeChar(&buffer_object, frameOut.payload.somedata);
    CIO_Buffer_SerializeLong(&buffer_object, frameOut.payload.rpm);
    CIO_Buffer_SerializeFloat(&buffer_object, frameOut.payload.temperature);
    // Payload - END

    // Calculate the CRC and write it to the buffer
    CIO_Buffer_CalculateCRC(&buffer_object, &frameOut.crc);
    CIO_Buffer_SerializeChar(&buffer_object, frameOut.crc);


    print_buffer_object(&buffer_object);

    /*
    CIO_Buffer_SerializeLong(&buffer_object, 123456);
    CIO_Buffer_SerializeLong(&buffer_object, 789);
    CIO_Buffer_SerializeChar(&buffer_object, 15);

    printf("Buffer Size: %d Bytes\n", buffer_object.offset);




    CIO_Buffer_Reset(&buffer_object);



    long long_val;
    char char_val;

    CIO_Buffer_DeserializeLong(&buffer_object, &long_val);
    printf("%ld\n", long_val);

    CIO_Buffer_DeserializeLong(&buffer_object, &long_val);
    printf("%ld\n", long_val);

    if(CIO_Buffer_DeserializeChar(&buffer_object, &char_val) == CIO_RESULT_GOOD)
    {
       printf("%d\n", char_val);
    }

*/

    return 0;
}
