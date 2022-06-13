#include "ring_buffer.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static size_t GetBufferReadRegions(RingBuffer* buf,
                                   size_t element_count,
                                   void** data_ptr_1,
                                   size_t* data_ptr_bytes_1,
                                   void** data_ptr_2,
                                   size_t* data_ptr_bytes_2) {

  const size_t readable_elements = Utility_available_read(buf);
  const size_t read_elements = (readable_elements < element_count ?
      readable_elements : element_count);
  const size_t margin = buf->element_count - buf->read_pos;

  // Check to see if read is not contiguous.
  if (read_elements > margin) {
    // Write data in two blocks that wrap the buffer.
    *data_ptr_1 = buf->data + buf->read_pos * buf->element_size;
    *data_ptr_bytes_1 = margin * buf->element_size;
    *data_ptr_2 = buf->data;
    *data_ptr_bytes_2 = (read_elements - margin) * buf->element_size;
  } else {
    *data_ptr_1 = buf->data + buf->read_pos * buf->element_size;
    *data_ptr_bytes_1 = read_elements * buf->element_size;
    *data_ptr_2 = NULL;
    *data_ptr_bytes_2 = 0;
  }

  return read_elements;
}

RingBuffer* Utility_CreateBuffer(size_t element_count, size_t element_size) {
  RingBuffer* rb = NULL;
  if (element_count == 0 || element_size == 0) {
    return NULL;
  }

  rb = (RingBuffer*)malloc(sizeof(RingBuffer));
  if (!rb) {
    return NULL;
  }

  rb->data = (char*)malloc(element_count * element_size);
  if (!rb->data) {
    free(rb);
    rb = NULL;
    return NULL;
  }

  rb->element_count = element_count;
  rb->element_size = element_size;
  Utility_InitBuffer(rb);

  return rb;
}

void Utility_InitBuffer(RingBuffer* rb) {
  rb->read_pos = 0;
  rb->write_pos = 0;
  rb->rw_wrap = SAME_WRAP;

  // Initialize buffer to zeros
  memset(rb->data, 0, rb->element_count * rb->element_size);
}

void Utility_FreeBuffer(void* handle) {
  RingBuffer* rb = (RingBuffer*)handle;
  if (!rb) {
    return;
  }

  free(rb->data);
  free(rb);
}

size_t Utility_ReadBuffer(RingBuffer* rb,
                         void** data_ptr,
                         void* data,
                         size_t element_count) {

  if (rb == NULL) {
    return 0;
  }
  if (data == NULL) {
    return 0;
  }

  {
    void* buf_ptr_1 = NULL;
    void* buf_ptr_2 = NULL;
    size_t buf_ptr_bytes_1 = 0;
    size_t buf_ptr_bytes_2 = 0;
    const size_t read_count = GetBufferReadRegions(rb,
                                                   element_count,
                                                   &buf_ptr_1,
                                                   &buf_ptr_bytes_1,
                                                   &buf_ptr_2,
                                                   &buf_ptr_bytes_2);
    if (buf_ptr_bytes_2 > 0) {
      // We have a wrap around when reading the buffer. Copy the buffer data to
      // |data| and point to it.
      memcpy(data, buf_ptr_1, buf_ptr_bytes_1);
      memcpy(((char*) data) + buf_ptr_bytes_1, buf_ptr_2, buf_ptr_bytes_2);
      buf_ptr_1 = data;
    } else if (!data_ptr) {
      // No wrap, but a memcpy was requested.
      memcpy(data, buf_ptr_1, buf_ptr_bytes_1);
    }
    if (data_ptr) {
      // |buf_ptr_1| == |data| in the case of a wrap.
      *data_ptr = read_count == 0 ? NULL : buf_ptr_1;
    }

    // Update read position
    Utility_MoveReadPtr(rb, (int) read_count);

    return read_count;
  }
}

size_t Utility_WriteBuffer(RingBuffer* rb,
                          const void* data,
                          size_t element_count) {
  if (!rb) {
    return 0;
  }
  if (!data) {
    return 0;
  }

  {
    const size_t free_elements = Utility_available_write(rb);
    const size_t write_elements = (free_elements < element_count ? free_elements
        : element_count);
    size_t n = write_elements;
    const size_t margin = rb->element_count - rb->write_pos;

    if (write_elements > margin) {
      // Buffer wrap around when writing.
      memcpy(rb->data + rb->write_pos * rb->element_size,
             data, margin * rb->element_size);
      rb->write_pos = 0;
      n -= margin;
      rb->rw_wrap = DIFF_WRAP;
    }
    memcpy(rb->data + rb->write_pos * rb->element_size,
           ((const char*) data) + ((write_elements - n) * rb->element_size),
           n * rb->element_size);
    rb->write_pos += n;

    return write_elements;
  }
}

int Utility_MoveReadPtr(RingBuffer* rb, int element_count) {
  if (!rb) {
    return 0;
  }

  {
    // We need to be able to take care of negative changes, hence use "int"
    // instead of "size_t".
    const int free_elements = (int) Utility_available_write(rb);
    const int readable_elements = (int) Utility_available_read(rb);
    int read_pos = (int) rb->read_pos;

    if (element_count > readable_elements) {
      element_count = readable_elements;
    }
    if (element_count < -free_elements) {
      element_count = -free_elements;
    }

    read_pos += element_count;
    if (read_pos > (int) rb->element_count) {
      // Buffer wrap around. Restart read position and wrap indicator.
      read_pos -= (int) rb->element_count;
      rb->rw_wrap = SAME_WRAP;
    }
    if (read_pos < 0) {
      // Buffer wrap around. Restart read position and wrap indicator.
      read_pos += (int) rb->element_count;
      rb->rw_wrap = DIFF_WRAP;
    }

    rb->read_pos = (size_t) read_pos;

    return element_count;
  }
}

size_t Utility_available_read(const RingBuffer* rb) {
  if (!rb) {
    return 0;
  }

  if (rb->rw_wrap == SAME_WRAP) {
    return rb->write_pos - rb->read_pos;
  } else {
    return rb->element_count - rb->read_pos + rb->write_pos;
  }
}

size_t Utility_available_write(const RingBuffer* rb) {
  if (!rb) {
    return 0;
  }

  return rb->element_count - Utility_available_read(rb);
}
