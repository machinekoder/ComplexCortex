#include "circularbuffer.h"

int8 Cb_initialize(CircularBuffer *buffer, uint16 bufferSize, uint16 dataSize, void *data)
{
    buffer->dataSize = dataSize;
    buffer->bufferSize = bufferSize * dataSize;
    buffer->start = 0u;
    buffer->count = 0u;
    
    if (data == NULL)
    {
        buffer->startPointer = calloc(bufferSize, dataSize);
    }
    else
    {
        buffer->startPointer = data;
    }
    
    if (buffer->startPointer == NULL)    //memory full
    {
        return (int8)(-1);
    }
    
    return (int8)(0);
}

int8 Cb_put(CircularBuffer *buffer, void *item)
{
    uint16 end;
    
    end = (buffer->start + buffer->count) % buffer->bufferSize;
    memcpy((void*)(&((uint8*)(buffer->startPointer))[end]), item, buffer->dataSize);
    
    if (buffer->count == buffer->bufferSize)
    {
        return (int8)(-1); // full
    }
    else
    {
        buffer->count += buffer->dataSize;
    }
    
    return (int8)(0);
}

int8 Cb_get(CircularBuffer *buffer, void *item)
{
    if (buffer->count == 0u)
    {
         return (int8)(-1); // buffer empty
    }
    
    memcpy(item, (void*)(&((uint8*)(buffer->startPointer))[buffer->start]), buffer->dataSize);
    buffer->start = (buffer->start + buffer->dataSize) % buffer->bufferSize;
    buffer->count -= buffer->dataSize;
    
    return (int8)(0);
}
