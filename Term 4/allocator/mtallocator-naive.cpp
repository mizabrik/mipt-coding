#include <cassert>
#include <cstdlib>

#include <algorithm>
#include <atomic>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

template <int SBSize>
class MtAllocator {
 public:
  MtAllocator(int heap_count)
    : heaps_(1 + heap_count) {}

  void * Allocate(size_t size) {
    if (size * 2 > SBSize) {
      return AllocateBig(size);
    }
    size_t sk = Log2(size);

    auto &heap = GetHeap();
    std::lock_guard<Heap> heap_lock(heap);

    void *block = heap.Allocate(sk);
    Superblock *sb = nullptr;
    if (block != nullptr) {
      return block;
    } else {
      std::lock_guard<Heap> global_lock(heaps_[0]);
      sb = heaps_[0].ReleaseSuperblock(sk);
    }

    if (sb == nullptr) {
      sb = new Superblock(sk);
    }

    block = sb->GetBlock();
    heap.AcquireSuperblock(sb);

    return block;
  }

  void Free(void *ptr) {
    if (ptr == nullptr)
      return;

    Superblock *sb = GetBlockOwner(ptr);
    if (sb == nullptr) {
      FreeBig(ptr);
      return;
    }

    Heap *heap = sb->GetOwner();
    std::unique_lock<Heap> heap_lock(*heap);
    while (heap != sb->GetOwner()) {
      heap_lock.unlock();
      heap = sb->GetOwner();
      std::unique_lock<Heap> new_heap_lock(*heap);
      std::swap(heap_lock, new_heap_lock);
    }

    heap->Free(ptr);

    if (heap == &heaps_[0]) {
      return;
    }

    auto allocated = heap->GetAllocated();
    auto used = heap->GetUsed();

    if (used < allocated - kReserveSize * SBSize
        && 2 * used < 1 * allocated) {
      std::lock_guard<Heap> global_lock(heaps_[0]);
      auto free_sb = heap->ReleaseSuperblock();
      if (free_sb)
        heaps_[0].AcquireSuperblock(free_sb);
    }
  }

 private:
  class Heap;

  class Superblock {
   public:
    Superblock(size_t size_kind)
      : block_size_(1 << size_kind),
        size_kind_(size_kind),
        used_(0),
        owner_(nullptr) {
      unsigned short block_count = SBSize / block_size_;
      data_ = std::malloc(sizeof(unsigned short) * block_count +
                          SBSize + sizeof(Superblock *) * block_count);
      next_blocks_ = (unsigned short *) data_;
      blocks_ = (uint8_t *) &next_blocks_[block_count];
      
      for (int i = 0; i < block_count; ++i) {
        next_blocks_[i] = i + 1;
        SetBlockOwner(GetBlockAddress(i), this);
      }
      next_blocks_[block_count - 1] = -1;
      free_block_ = 0;
    }

    ~Superblock() {
      std::free(data_);
    }

    size_t GetBlockSize() const {
      return block_size_;
    }

    void * GetBlock() {
      assert(used_ < SBSize);
      void *block = GetBlockAddress(free_block_);
      free_block_ = next_blocks_[free_block_];

      used_ += block_size_;

      return (void *) block;
    }

    void FreeBlock(void *addr) {
      unsigned int block = ((uint8_t *) addr - blocks_ - sizeof(Superblock *))
                           / (block_size_ + sizeof(Superblock *));
      next_blocks_[block] = free_block_;
      free_block_ = block;
      used_ -= block_size_;
    }

    size_t GetSizeKind() const {
      return size_kind_;
    }

    Heap * GetOwner() const {
      return owner_;
    }

    void SetOwner(Heap *new_owner) {
      owner_ = new_owner;
    }

    size_t GetUsed() const {
      return used_;
    }

   private:
    void * GetBlockAddress(size_t id) {
      return (uint8_t *) blocks_ + id * block_size_
                                 + (id + 1) * sizeof(Superblock *);
    }

    const size_t block_size_;
    const size_t size_kind_;

    size_t used_;

    std::atomic<Heap *> owner_;

    void *data_;
    unsigned short *next_blocks_;
    unsigned short free_block_;
    uint8_t *blocks_;
  };

  class Heap {
   public:
    Heap()
     : superblocks_(Log2(SBSize)),
       last_freed_(Log2(SBSize), nullptr),
       position_(Log2(SBSize), 0),
       allocated_(0),
       used_(0) {}

    ~Heap() {
      for (auto &sk_superblocks : superblocks_) {
        for (auto sb : sk_superblocks) {
          delete sb;
        }
      }
    }

    void * Allocate(size_t size_kind) {
      auto &lf = last_freed_[size_kind];
      if (lf) {
        assert(lf->GetUsed() < SBSize);
        void *block = lf->GetBlock();
        if (lf->GetUsed() == SBSize) {
          lf = nullptr;
        }
        return block;
      }

      auto &sk_superblocks = superblocks_[size_kind];
      auto sb_count = sk_superblocks.size();
      for (size_t shift = 0; shift < sb_count; ++shift) {
        size_t i = (position_[size_kind] + shift) % sb_count;
        auto sb = sk_superblocks[i];
        if (sb->GetUsed() < SBSize) {
          void *block = sb->GetBlock();
          used_ += 1 << size_kind;
          position_[size_kind] = i;
          return block;
        }
      }

      return nullptr;
    }

    void Free(void *block) {
      auto sb = GetBlockOwner(block);
      sb->FreeBlock(block);

      last_freed_[sb->GetSizeKind()] = sb;

      used_ -= sb->GetBlockSize();
    }

    void AcquireSuperblock(Superblock *sb) {
      sb->SetOwner(this);
      auto sk = sb->GetSizeKind();

      superblocks_[sk].push_back(sb);
      std::swap(superblocks_[sk].front(), superblocks_[sk].back());
      allocated_ += SBSize;
      used_ += sb->GetUsed();
    }

    Superblock * ReleaseSuperblock(size_t size_kind) {
      if (superblocks_[size_kind].empty()) {
        return nullptr;
      } else {
        auto sb = superblocks_[size_kind].back();
        superblocks_[size_kind].pop_back();
        ReleaseSuperblock(sb);
        return sb;
      }
    }

    Superblock * ReleaseSuperblock() {
      for (auto &sk_superblocks : superblocks_) {
        for (auto &candidate : sk_superblocks) {
          if (candidate->GetUsed() < SBSize) {
            auto sb = candidate;
            std::swap(candidate, sk_superblocks.back());
            sk_superblocks.pop_back();
            ReleaseSuperblock(sb);
            return sb;
          }
        }
      }

      return nullptr;
    }

    size_t GetAllocated() const {
      return allocated_;
    }

    size_t GetUsed() const {
      return used_;
    }

    void lock() {
      lock_.lock();
    }
    
    void unlock() {
      lock_.unlock();
    }

   private:
    void ReleaseSuperblock(Superblock *sb) {
      allocated_ -= SBSize;
      used_ -= sb->GetUsed();
      auto sk = sb->GetSizeKind();
      if (last_freed_[sk] == sb)
        last_freed_[sk] = nullptr;
    }

    std::mutex lock_;

    std::vector<std::vector<Superblock *>> superblocks_;
    std::vector<Superblock *> last_freed_;
    std::vector<size_t> position_;

    size_t allocated_;
    size_t used_;
  };

  size_t kReserveSize = 5;

  Heap & GetHeap() {
    auto heap_id = hasher(std::this_thread::get_id()) % (heaps_.size() - 1);
    return heaps_[heap_id + 1];
  }

  void * AllocateBig(size_t size) {
    uint8_t *data = (uint8_t *) std::malloc(size + sizeof(Superblock *));

    if (data != nullptr) {
      data += sizeof(Superblock *);
      SetBlockOwner(data, nullptr);
      return data;
    } else {
      return nullptr;
    }
  }

  void FreeBig(void *data) {
    std::free((uint8_t *) data - sizeof(Superblock *));
  }

  static Superblock * GetBlockOwner(void *block) {
    return ((Superblock **) block)[-1];
  }

  static void SetBlockOwner(void *block, Superblock *owner) {
    ((Superblock **) block)[-1] = owner;
  }

  static size_t Log2(size_t n) {
    size_t power = 0;
    size_t exp = 1;
    while (exp < n) {
      ++power;
      exp *= 2;
    }
    return power;
  }

  std::hash<std::thread::id> hasher;

  std::vector<Heap> heaps_;

  int heap_count_;
};

static MtAllocator<1 << 14> & GetAllocator() {
  static int heap_count = std::max(std::thread::hardware_concurrency(), 1U) * 2;
  static MtAllocator<1 << 14> allocator(heap_count);
  return allocator;
}

void * mtalloc(size_t bytes) {
  return GetAllocator().Allocate(bytes);
}

void mtfree(void* ptr) {
  GetAllocator().Free(ptr);
}
