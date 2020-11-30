#pragma once

#include <cstdint>
#include <map>
#include <GL/glew.h>

// Handles management of OpenGL VBOs so that many geometries may share the same buffer,
// avoiding an excessive number of tiny allocations.
class GLMemPool {

  // for internal size tracking
  struct BufferBlock {
    size_t capacity;
    size_t size;
    GLuint name;
  };

public:
  // for outside classes to receive
  struct GLBuffer {
    GLuint name;
    size_t offset;
  };

  GLMemPool(size_t block_bytes=1024*1024) : default_block_bytes(block_bytes) { }
  ~GLMemPool();
  GLMemPool(const GLMemPool&) = delete;
  GLMemPool(GLMemPool&&) noexcept = delete;
  GLMemPool& operator=(const GLMemPool& other) = delete;
  GLMemPool& operator=(GLMemPool&& other) noexcept = delete;

  bool requestBuffer(size_t size, GLBuffer& buf_out);
  // reset block sizes in pool but leave buffers allocated in GPU RAM for reuse
  void clear();

private:
  const size_t default_block_bytes;
  // pool of existing GL buffers, sorted by space remaining
  std::multimap<size_t, BufferBlock> pool;
};
