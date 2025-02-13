#include "ring_buffer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Queue Reset
void RingBuffer_Reset(Ring_Buffer_t *ringBuf)
{
    // The head pointer and tail pointer both point to the first address of the buffer.
    ringBuf->head = 0;
    ringBuf->tail = 0;
    // Initialize the value to 0
    for (uint16_t i = 0; i < ringBuf->size; i++)
    {
        ringBuf->buffer[i] = 0;
    }
}

// Creating a Queue
void RingBuffer_Init(Ring_Buffer_t *ringBuf, uint16_t capacity)
{
    // The size needs to be larger than the capacity.
    ringBuf->size = capacity + 1;

    // Buffer memory allocation
    ringBuf->buffer = (uint8_t *)malloc(ringBuf->size);
    
    // Reset Queue Data
    RingBuffer_Reset(ringBuf);
    // printf("[RingBuffer_Init] ring buffer init pbuffer=%p\n", ringBuf->buffer);
}

// Destroy Queue
void RingBuffer_Destory(Ring_Buffer_t *ringBuf)
{
	free(ringBuf->buffer);
	free(ringBuf);
}

// Get the capacity of a ring queue
uint16_t RingBuffer_Get_Capacity(Ring_Buffer_t *ringBuf)
{
    return ringBuf->size - 1;
}

// Get the number of bytes stored in the circular queue
uint16_t RingBuffer_Get_Used_Count(Ring_Buffer_t *ringBuf)
{
    if (ringBuf->head > ringBuf->tail)
    {
        return RingBuffer_Get_Capacity(ringBuf) - (ringBuf->head - ringBuf->tail - 1);
    }
    else
    {
        return ringBuf->tail - ringBuf->head;
    }
}

// Get the remaining capacity of a queue
uint16_t RingBuffer_Get_Free_Count(Ring_Buffer_t *ringBuf)
{
    return RingBuffer_Get_Capacity(ringBuf) - RingBuffer_Get_Used_Count(ringBuf);
}

// Determine whether the circular queue is empty
uint8_t RingBuffer_IsEmpty(Ring_Buffer_t *ringBuf)
{
    return RingBuffer_Get_Free_Count(ringBuf) == RingBuffer_Get_Capacity(ringBuf);
}

// Determine whether the ring queue is full
uint8_t RingBuffer_IsFull(Ring_Buffer_t *ringBuf)
{
    return RingBuffer_Get_Free_Count(ringBuf) == 0;
}

// Read the i-th element according to the index number
uint8_t RingBuffer_GetValue_ByIndex(Ring_Buffer_t *ringBuf, uint16_t index)
{
    if (index >= RingBuffer_Get_Used_Count(ringBuf))
    {
        // Index number out of range
        return 0;
    }
    uint16_t rbIdx = (ringBuf->head + index + 1) % ringBuf->size;
    return ringBuf->buffer[rbIdx];
}

// Clear queue data (read out all stored data in the ring buffer)
void RingBuffer_Clean_Queue(Ring_Buffer_t *ringBuf)
{
    while (RingBuffer_Get_Used_Count(ringBuf))
    {
        RingBuffer_Pop(ringBuf);
    }
}

// Pop up the first element of the queue
uint8_t RingBuffer_Pop(Ring_Buffer_t *ringBuf)
{
    uint8_t temp = 0;
    if (!RingBuffer_IsEmpty(ringBuf))
    {
        ringBuf->head = (ringBuf->head + 1) % ringBuf->size;
        temp = ringBuf->buffer[ringBuf->head];
        // Clear after popping the first element
        ringBuf->buffer[ringBuf->head] = 0;
    }
    return temp;
}

// Write the tail element
void RingBuffer_Push(Ring_Buffer_t *ringBuf, uint8_t value)
{
    if (RingBuffer_IsFull(ringBuf))
    {
        // The queue is full, so we can only pop the element at the head of the queue.
        RingBuffer_Pop(ringBuf);
    }
    ringBuf->tail = (ringBuf->tail + 1) % ringBuf->size;
    ringBuf->buffer[ringBuf->tail] = value;

    // if (!RingBuffer_IsFull(ringBuf))
    // {
    //     ringBuf->tail = (ringBuf->tail + 1) % ringBuf->size;
    //     ringBuf->buffer[ringBuf->tail] = value;
    // }
}


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

// 读取单个字节
uint8_t RingBuffer_ReadByte(Ring_Buffer_t *ringBuf)
{
    return RingBuffer_Pop(ringBuf);
}

// Reading a single byte
void RingBuffer_ReadByteArray(Ring_Buffer_t *ringBuf, uint8_t *dest, uint16_t size)
{
    for (uint16_t idx = 0; idx < size; idx++)
    {
        dest[idx] = RingBuffer_Pop(ringBuf);
    }
}

// Writing a single byte
void RingBuffer_WriteByte(Ring_Buffer_t *ringBuf, uint8_t value)
{
    RingBuffer_Push(ringBuf, value);
}

// Writing Byte Arrays
void RingBuffer_WriteByteArray(Ring_Buffer_t *ringBuf, uint8_t *src, uint16_t size)
{
    for (uint16_t idx = 0; idx < size; idx++)
    {
        RingBuffer_Push(ringBuf, src[idx]);
    }
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////


// Read a signed short integer (two bytes)
int16_t RingBuffer_ReadShort(Ring_Buffer_t *ringBuf)
{
    int16_t value;
    uint8_t *p = (uint8_t *)&value;
    RingBuffer_ReadByteArray(ringBuf, p, 2);
    return value;
}

// Read an unsigned short integer (two bytes)
uint16_t RingBuffer_ReadUShort(Ring_Buffer_t *ringBuf)
{
    uint16_t value;
    uint8_t *p = (uint8_t *)&value;
    RingBuffer_ReadByteArray(ringBuf, p, 2);
    return value;
}

// Read a signed Long integer (four bytes)
int32_t RingBuffer_ReadLong(Ring_Buffer_t *ringBuf)
{
    int32_t value;
    uint8_t *p = (uint8_t *)&value;
    RingBuffer_ReadByteArray(ringBuf, p, 4);
    return value;
}

// Read an unsigned Long integer (four bytes)
uint32_t RingBuffer_ReadULong(Ring_Buffer_t *ringBuf)
{
    uint32_t value;
    uint8_t *p = (uint8_t *)&value;
    RingBuffer_ReadByteArray(ringBuf, p, 4);
    return value;
}

// Read floating point number (four bytes)
float RingBuffer_ReadFloat(Ring_Buffer_t *ringBuf)
{
    float value;
    uint8_t *p = (uint8_t *)&value;
    RingBuffer_ReadByteArray(ringBuf, p, 4);
    return value;
}


// Write a signed short integer (two bytes)
void RingBuffer_WriteShort(Ring_Buffer_t *ringBuf, int16_t value)
{
    uint8_t *p = (uint8_t *)&value;
    RingBuffer_WriteByteArray(ringBuf, p, 2);
}

// Write unsigned short integer (two bytes)
void RingBuffer_WriteUShort(Ring_Buffer_t *ringBuf, uint16_t value)
{
    uint8_t *p = (uint8_t *)&value;
    RingBuffer_WriteByteArray(ringBuf, p, 2);
}

// Write a signed Long integer (four bytes)
void RingBuffer_WriteLong(Ring_Buffer_t *ringBuf, int32_t value)
{
    uint8_t *p = (uint8_t *)&value;
    RingBuffer_WriteByteArray(ringBuf, p, 4);
}

// Write an unsigned Long integer (four bytes)
void RingBuffer_WriteULong(Ring_Buffer_t *ringBuf, uint32_t value)
{
    uint8_t *p = (uint8_t *)&value;
    RingBuffer_WriteByteArray(ringBuf, p, 4);
}

// Write floating point number (four bytes)
void RingBuffer_WriteFloat(Ring_Buffer_t *ringBuf, float value)
{
    uint8_t *p = (uint8_t *)&value;
    RingBuffer_WriteByteArray(ringBuf, p, 4);
}