// SPDX-License-Identifier: GPL-3.0-or-later

#include <cstring>

#include "ring_buffer.hpp"

namespace lm {

RingBuffer::RingBuffer() 
  : buffer()
  , head(0)
  , tail(0)
{
  memset(buffer, '\0', RING_BUFFER_LENGTH);
}

bool RingBuffer::write(uint8_t data) {
  // The next location we are going to write into
  // the buffer is the next location in the array
  // from the head. as this is a ring buffer, we
  // wrap from end to beginning when we would linearly
  // write past the end of the array.
  uint32_t next_head = (head + 1) % RING_BUFFER_LENGTH;
  
  // if the next location we are going to write at is
  // equal to the tail of the buffer, then the buffer
  // is full, so we cannot write any more data.
  if (next_head == tail) { return false; }

  buffer[head] = data;
  head         = next_head;
  return true;
}

bool RingBuffer::write(uint8_t *data, uint32_t length) {
  // the space we have to fit the data into the buffer
  // is equal to the distance between the head and the tail.
  // modulo the size of the buffer. however, since this is a
  // ring buffer, we know we are in a situation where:
  //
  //   1. the head is ahead of the tail
  //   2. the head is behind the tail.
  //   3. the head and the tail are equal
  //
  // if the head is ahead of the tail then we have space between 
  // the current head and the end of the array, plus the space 
  // between the beginning of the array and the tail.
  //
  // [ | | | |x|x|x|x|x|x|x|x|x|x|x|x|x|x|x| | | | | | | | | | ]
  //          ^                             ^
  //         tail                          head
  //
  // if the head is behind the tail then we have space between the 
  // head and the tail
  //
  // [x|x|x|x|x| | | | | | | | | | | | |x|x|x|x|x|x|x|x|x|x|x|x]
  //            ^                       ^
  //          head                    tail
  //
  // if the head and the tail are equal, then we know the buffer is 
  // empty. so we have the space between the head and the end of the 
  // array, plus the space between the beginning of the array and the 
  // tail.
  //
  // [ | | | | | | | | | | | | | | | | | | | | | | | | | | | | ]
  // ^^
  // head&tail
  //
  // since we don't have to worry about wraparound in situation 2.
  // (you should be able to prove this to yourself by considering 
  //  all possible arrangements of head and tail where we maintain
  //  the invariant that head is less than tail)
  // then this is the simplest case programatically, and in order to 
  // keep the code as flat as we can, I will nest this case into 
  // the if body and let situation 1 be handled after the if statement.
  //

  if (head < tail) {
    // situation 2.
    // the space available to fit the string is equal to the distance
    // between head and tail. where we must take care not to write any
    // data at tail, as this byte is already occupied.
    // since we know that head < tail. we know that we will get a positive 
    // result from subtracting head from tail. so it's safe to store within 
    // a unsigned integer.
    //
    uint32_t available = tail - head;

    // if the available space is equal to the length, then we know it is
    // safe to store that many bytes into the buffer. due to zero indexing
    // the space right at head is index 0. So if we have 4 bytes of space
    // available, and we want to write 4 bytes, we will be safe, as we write
    // byte 0, 1, 2, and 3 into their positions, and byte 4 is at tail.
    // meaning we have not overwritten tail. if the amount we have available
    // is 3, and we want to write 4, then bytes, 0, 1, 2 will be fine, and
    // then byte 3 will overwrite the byte currently at the tail. so we
    // cannot fit the entire input into the buffer, so we return false.
    if (available < length) { return false; }

    memcpy(buffer + head, data, length);
    // Notice here that since tail is ahead of the head, there is no way
    // for head to have to wrap around the end of the array, so we know
    // that we can just increment head, and we don't need to account for
    // the wrapping around the array. which means we can further save
    // ourselves from having to perform an expensive division operation.
    head += length;
    return true;
  }

  // situation 1, or 3. Notice that we don't need to distinguish beteen
  // these two situations in the code, as they have the same invariants.
  // we need to take care to wrap the new data around the buffer either
  // way. in order to wrap the data around the buffer, we break this 
  // into two situations.
  //
  //    1. writing data into the buffer from head to LENGTH
  //    2. writing data into the buffer from beginning to tail
  // 
  // when we are writing data from head to LENGTH, we have an amount 
  // available for writing equal to that distance
  //
  // [ | | | | | | |x|x|x|x|x|x|x|x|x|x|x| | | | | | | | | | | ]
  //                ^                     ^                     ^ 
  //              tail                   head                  LENGTH
  //
  // when we consider writing data into the buffer in situation 1. we have 
  // two further situations
  //
  //   1. the new data fits into the space at the end
  //   2. the new data overruns the space at the end
  // 
  // if the new data fits into the space at the end of the array, then we
  // can write that data into the end of the array, and we are done. so we 
  // may then return early. since this is an easy case, we want to place the 
  // logic of this into a guard clause
  //
  // if the new data does not fit into the space available at the end of the 
  // array, we may still be able to fit the new data into the buffer by utilizing 
  // the space available between the beginning of the buffer and the tail.
  //

  // Notice that since LENGTH is always greater than head, we are
  // always going to get a positive result when we subtract head from
  // LENGTH, so we are safe to store it into a unsigned integer.
  uint32_t available_at_end = RING_BUFFER_LENGTH - head;

  // this is just the length available at the end of the buffer however.
  // if this is enough to fit the given data, then we are in another 
  // easy situation, so we will place that logic into the guard clause.
  //
  // Notice that if available_at_end is strictly greater than length,
  // we will not have to wrap the head around the end of the array,
  // since this case allows us to complete our work without further 
  // conditions, we want to place it's logic into the guard clause
  if (available_at_end >= length) {
    memcpy(buffer + head, data, length);
    // notice that in this case if the length is equal to available at end
    // then we will have to wrap the head around to the beginning. we know 
    // that we will be wrapping to the first slot, it's just that in order 
    // to tell if we wrap or not we have to use another condition, or we 
    // could always divide. which is faster is not visible without measurement,
    // because in either case we always check a branch, or we always divide.
    // there is no branch predictor on the sam3x8e as far as I am aware,
    // and division is famously a slow operation. whereas integral comparison
    // is usually faster.
    // so I am leading myself to think that 
    //
    //   `head = (head == available_at_end) ? buffer : (head + length);`
    //
    // would be faster by a few clock cycles than
    //
    //   `head = (head + length) % LENGTH;`
    //
    head = (head == available_at_end) ? 0 : (head + length);
    return true;
  }

  // situation 2.2.
  // we are adding to the buffer at the end, and we will only be able to 
  // write part of the data there, the rest will have to fit into the 
  // space at the beginning.
  //
  // [ | | | | | | |x|x|x|x|x|x|x|x|x|x|x| | | | | | | | | | | ]
  //                ^                     ^                     ^ 
  //              tail                   head                  LENGTH
  //
  // the space at the beginning of the array looks a lot like the 
  // space within an entire array, if we take the tail pointer to 
  // be "one-past-the-end" as it were. This means that the space 
  // available at the beginning of the array is equal to the tail.
  // (since we are storing the tail as an index rather than a pointer.
  //  were it a pointer we would have to subtract beginning from tail
  //  to derive the space available).
  //
  // I think we have to subtract one from tail. becuase when we write 
  // data to the beginning, if we have to write exactly as much as is 
  // available at the beginning to the beginning, then head would equal
  // tail at that point. which breaks the invariant that head == tail
  // means the buffer is empty, and head + 1 == tail means the buffer 
  // is full. 
  //
  //

  uint32_t available_at_beginning = tail;
  uint32_t available = available_at_end + available_at_beginning;
  if (available < length) { return false; }
  
  // we know that we can only write part of the data to the end, and 
  // the other part to the beginning. so we first write what we can to 
  // the end.
  memcpy(buffer + head, data, available_at_end);

  // then what we have remaining is the difference between what we wrote
  // and whats left.
  uint32_t remaining = length - available_at_end;
  // so we can write out what is remaining of the data
  memcpy(buffer, data + available_at_end, remaining);
  // and place the head at the location just after the last byte written
  head = remaining;
  return true;
}

bool RingBuffer::read(uint8_t *data) {
  // We want to read a single byte from the buffer, and store it into data.
  // we read bytes from the tail of the buffer, 
}

uint32_t RingBuffer::read(uint8_t *data, uint32_t length) {

}

} // namespace lm
