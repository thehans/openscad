#include <vector>

#include "GLMemPool.h"
#include "system-gl.h"

bool GLMemPool::requestBuffer(size_t size, GLBuffer& buf_out) {
  auto it = pool.lower_bound(size);
  if (it != pool.end()) {
    BufferBlock block = it->second;
    pool.erase(it);
    buf_out.name = block.name;
    buf_out.offset = block.size;
    block.size += size;
    pool.emplace(block.capacity - block.size, block);
    return true;
  }

  size_t capacity = std::max(size, default_block_bytes);

  GLuint name;
  glGenBuffers(1, &name);
  if (report_glerror("GLMemPool::requestBuffer glGenBuffers")) return false;

  glNamedBufferData(name, capacity, nullptr, GL_STATIC_DRAW);
  if (report_glerror("GLMemPool::requestBuffer glNamedBufferData")) return false;

  pool.emplace(capacity - size, BufferBlock{capacity, size, name} );
  buf_out.name = name;
  buf_out.offset = 0;
  return true;
}

void GLMemPool::clear() {
  std::multimap<size_t, BufferBlock> temp;
  for (const auto &kvp : this->pool) {
    auto block = kvp.second;
    block.size = 0;
    temp.emplace(block.capacity, block);
  }
  std::swap(temp, this->pool);
}

GLMemPool::~GLMemPool() {
  std::vector<GLuint> buffers;
  for (const auto &kvp : this->pool) {
    buffers.push_back(kvp.second.name);
  }
  this->pool.clear();
  if (!buffers.empty()) {
    glDeleteBuffers(buffers.size(), buffers.data());
  }
}
