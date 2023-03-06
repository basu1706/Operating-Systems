// Memory allocator that maintains a free list of memory blocks and uses first fit strategy to allocate memory
// To run: clang++ free_list_allocator.cpp -o allocator followed by ./allocator
#include <iostream>
#include <unistd.h>
#include <assert.h>


/*
 intptr_t  is  a signed integer type such that any valid (void *) value can be converted to
this type and then converted  back.   It  is  capable  of  storing  values  in  the  range
[INTPTR_MIN, INTPTR_MAX].
 */
using word_t = intptr_t;
 
// Allocated chunk of memory
struct Block {
  size_t size;
  bool is_free;
  // Pointer to the next memory block
  Block *next;
  // Payload data
  word_t data[1];
 
};

static Block* first=nullptr;
static Block* last_alloc=first;

/**
 * Aligns the size by the machine word.
 */
inline size_t align(size_t n) {
  return (n + sizeof(word_t) - 1) & ~(sizeof(word_t) - 1);
}

// Running the first-fit algorithm returns the first free memory block that has size >= requested size. If not, return null
Block* findFirstFit(size_t size){
  Block* pointer = first;
  while (pointer!=nullptr){
    if (pointer->is_free==false || pointer->size < size){
      pointer=pointer->next;
      continue;
    }
    return pointer;
  }
  return nullptr;
}


Block* allocate(size_t size) {

  size=size+sizeof(Block);
  // Gets the Current heap break.
  Block* block = (Block *)sbrk(0);
  // Call sbrk() passing the size in bytes by which the break position needs to be increased. If the call returns -1, nullptr is returned, otherwise the address of the beginning of the new allocated block is returned           
  if (sbrk(size) == (void *)-1) { 
      // If allocation fails, return null, else return a pointer to the block
    return nullptr;
  }
 
  return block;
}
// Call the allocate function to allocate size bytes and update the heap pointers accordingly
word_t* alloc(size_t size){
    size = align(size);

    if (Block* block = findFirstFit(size))
        return block->data;
    
    Block* block = allocate(size);
    block->size = size;
    block->is_free = false;
    
    // Update first and last_alloc pointers
    if (first == nullptr) {
        first = block;
    }
    
    if (last_alloc != nullptr) {
        last_alloc->next = block;
    }
    
    last_alloc = block;  
    return block->data;

}

/**
 * Returns the object header.
 */
Block *getHeader(word_t *data) {
  std::cout  << (char *)data << std::endl;
  return (Block *)((char *)data + sizeof(std::declval<Block>().data) -
                   sizeof(Block));
}

// Freeing the allocated data
void free(word_t* data){
    Block* header=getHeader(data);
    header->is_free=true;
    return;
}

// Some test cases
int main(int argc, char const *argv[]) {
 
  // --------------------------------------
  // Test case 1: Alignment
  //
  // A request for 3 bytes is aligned to 8.
  word_t* block1 = alloc(3);                        // (1)
  Block* block1_header = getHeader(block1);
  assert(block1_header->size == sizeof(word_t));
 
  // Test case 2: Freeing the object
  free(block1);
  assert(block1_header->is_free == true);

  // Test case 3: After the memory block is freed, the same is used for a consequent allocation
  word_t* block2 = alloc(8);
  Block* block2_header = getHeader(block2);
  assert(block2_header->size==8);
  assert(block2_header==block1_header);
 
  puts("\nAll assertions passed!\n");
 
}