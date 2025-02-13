#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"
#include "stdint.h"

// Ring queue structure
typedef struct
{  
    uint8_t *buffer; // Buffer
    uint16_t head;   // Head pointer
    uint16_t tail;   // Tail pointer
    uint16_t size;   // Size of the ring queue
} Ring_Buffer_t;

// Creating a Queue
void RingBuffer_Init(Ring_Buffer_t* ringBuf, uint16_t capacity);
// Queue Reset
void RingBuffer_Reset(Ring_Buffer_t *ringBuf);
// Destroy Queue
void RingBuffer_Destory(Ring_Buffer_t *ringBuf);
// Get the capacity of a queue
uint16_t RingBuffer_Get_Capacity(Ring_Buffer_t *ringBuf);
// Get the number of bytes stored in the circular queue
uint16_t RingBuffer_Get_Used_Count(Ring_Buffer_t *ringBuf);
// Get the remaining free bytes of the queue
uint16_t RingBuffer_Get_Free_Count(Ring_Buffer_t *ringBuf);
// Is the queue empty?
uint8_t RingBuffer_IsEmpty(Ring_Buffer_t *ringBuf);
// Is the queue full?
uint8_t RingBuffer_IsFull(Ring_Buffer_t *ringBuf);
// Read the i-th element according to the index number
uint8_t RingBuffer_GetValue_ByIndex(Ring_Buffer_t *ringBuf, uint16_t index);
// Clear queue data (read out all stored data in the ring buffer)
void RingBuffer_Clean_Queue(Ring_Buffer_t *ringBuf);
// Write the tail element (push on the stack)
void RingBuffer_Push(Ring_Buffer_t *ringBuf, uint8_t value);
// First element (pop)
uint8_t RingBuffer_Pop(Ring_Buffer_t *ringBuf);

// Reading a single byte
uint8_t RingBuffer_ReadByte(Ring_Buffer_t *ringBuf);
// Read byte array
void RingBuffer_ReadByteArray(Ring_Buffer_t *ringBuf, uint8_t* dest, uint16_t size);
// Read a signed short integer (two bytes)
int16_t RingBuffer_ReadShort(Ring_Buffer_t *ringBuf);
// Read an unsigned short integer (two bytes)
uint16_t RingBuffer_ReadUShort(Ring_Buffer_t *ringBuf);
// Read a signed Long integer (four bytes)
int32_t RingBuffer_ReadLong(Ring_Buffer_t *ringBuf);
// Read an unsigned Long integer (four bytes)
uint32_t RingBuffer_ReadULong(Ring_Buffer_t *ringBuf);
// Read floating point number (four bytes)
float RingBuffer_ReadFloat(Ring_Buffer_t *ringBuf);

// Writing a single byte
void RingBuffer_WriteByte(Ring_Buffer_t *ringBuf, uint8_t value);
// Writing Byte Arrays
void RingBuffer_WriteByteArray(Ring_Buffer_t *ringBuf, uint8_t* src, uint16_t size);
// Write a signed short integer (two bytes)
void RingBuffer_WriteShort(Ring_Buffer_t *ringBuf, int16_t value);
// Write unsigned short integer (two bytes)
void RingBuffer_WriteUShort(Ring_Buffer_t *ringBuf, uint16_t value);
// Write a signed Long integer (four bytes)
void RingBuffer_WriteLong(Ring_Buffer_t *ringBuf, int32_t value);
// Write an unsigned Long integer (four bytes)
void RingBuffer_WriteULong(Ring_Buffer_t *ringBuf, uint32_t value);
// Write floating point number (four bytes)
void RingBuffer_WriteFloat(Ring_Buffer_t *ringBuf, float value);

#ifdef __cplusplus
}
#endif