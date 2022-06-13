#ifndef UTILITY_RING_BUFFER_H_
#define UTILITY_RING_BUFFER_H_

#include <stddef.h>
#include <memory>
enum Wrap { SAME_WRAP, DIFF_WRAP };
#ifndef size_t
typedef unsigned int size_t;
#endif // !size_t

typedef struct RingBuffer {
    size_t read_pos;
    size_t write_pos;
    size_t element_count;
    size_t element_size;
    enum Wrap rw_wrap;
    char* data;
} RingBuffer;




RingBuffer* Utility_CreateBuffer(size_t element_count, size_t element_size);
void Utility_InitBuffer(RingBuffer* handle);
void Utility_FreeBuffer(void* handle);


size_t Utility_ReadBuffer(RingBuffer* handle,
    void** data_ptr,
    void* data,
    size_t element_count);
size_t Utility_WriteBuffer(RingBuffer* handle,
    const void* data,
    size_t element_count);


int Utility_MoveReadPtr(RingBuffer* handle, int element_count);
size_t Utility_available_read(const RingBuffer* handle);
size_t Utility_available_write(const RingBuffer* handle);

#endif  // UTILITY_RING_BUFFER_H_
