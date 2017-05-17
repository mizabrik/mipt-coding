#include <cassert>
#include <cstdlib>

#include <algorithm>
#include <atomic>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

#include <iostream>
// #include <sys/mman.h>

template <int SBSize, int FullnessKinds = 5>
class MtAllocator {
 public:
  MtAllocator(int heap_count)
    : heaps_(1 + heap_count) {}

  void stats() {
    int i = 0;
    for (auto &h : heaps_) {
      std::cout << (i++) << ' ' << h.GetUsed()
                << '/' << h.GetAllocated() << std::endl;
    }
  }

  void * Allocate(size_t size) {
    if (size * 2 > SBSize) {
      return AllocateBig(size);
    }

    auto &heap = GetHeap();
    std::lock_guard<Heap> heap_lock(heap);
    size_t sk = Log2(size);

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

    std::lock_guard<Superblock> sb_lock(*sb);
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

    //return;

    auto allocated = heap->GetAllocated();
    auto used = heap->GetUsed();

    if (used < allocated - kReserveSize * SBSize
        && 4 * used < 3 * allocated) {
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
        owner_(nullptr),
        next_(nullptr),
        prev_(nullptr) {
      unsigned short block_count = SBSize / block_size_;
      data_ = Mmap(sizeof(unsigned short) * block_count +
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
      Munmap(data_);
    }

    size_t GetBlockSize() const {
      return block_size_;
    }

    Superblock * GetNext() const {
      return next_;
    }

    void SetNext(Superblock *next) {
      next_ = next;
    }

    Superblock * GetPrev() const {
      return prev_;
    }

    void SetPrev(Superblock *prev) {
      prev_ = prev;
    }

    void * GetBlock() {
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

    // Returns value from 0 to FullnessKinds + 1
    int GetFullnessKind() const {
      if (used_ == 0) {
        return 0;
      } else {
        return 1 + (FullnessKinds * used_) / SBSize;
      }
    }

    size_t GetSizeKind() const {
      return size_kind_;
    }

    void SetOwner(Heap *new_owner) {
      owner_ = new_owner;
    }

    size_t GetUsed() const {
      return used_;
    }

    Heap * GetOwner() const {
      return owner_;
    }

    void lock() {
      lock_.lock();
    }
    
    void unlock() {
      lock_.unlock();
    }

   private:
    void * GetBlockAddress(size_t id) {
      return (uint8_t *) blocks_ + id * block_size_
                                 + (id + 1) * sizeof(Superblock *);
    }

    std::mutex lock_;

    const size_t block_size_;
    const size_t size_kind_;

    size_t used_;

    std::atomic<Heap *> owner_;

    Superblock *next_;
    Superblock *prev_;

    void *data_;
    unsigned short *next_blocks_;
    unsigned short free_block_;
    uint8_t *blocks_;
  };

  class Heap {
   public:
    Heap()
     : superblocks_(Log2(SBSize),
                    std::vector<Superblock *>(FullnessKinds + 2, nullptr)),
       allocated_(0),
       used_(0) {}

    ~Heap() {
      for (auto &sk_superblocks : superblocks_) {
        for (auto &list : sk_superblocks) {
          while (list) {
            auto next = list->GetNext();
            delete list;
            list = next;
          }
        }
      }
    }

    void * Allocate(size_t size_kind) {
      auto &superblocks = superblocks_[size_kind];

      for (int fk = FullnessKinds; fk > 0; --fk) {
        Superblock *sb = superblocks[fk];
        if (sb != nullptr) {
          void *block = sb->GetBlock();

          auto new_fk = sb->GetFullnessKind();
          if (new_fk != fk) {
            PopSuperblock(superblocks[fk]);
            PushSuperblock(superblocks[new_fk], sb);
          }

          used_ += sb->GetBlockSize();

          return block;
        }
      }

      return nullptr;
    }

    void Free(void *block) {
      auto sb = GetBlockOwner(block);

      auto sk = sb->GetSizeKind();
      auto fk = sb->GetFullnessKind();

      sb->FreeBlock(block);

      if (sb->GetPrev() != nullptr) {
        CutSuperblock(sb);
      } else {
        PopSuperblock(superblocks_[sk][fk]);
      }

      auto new_fk = sb->GetFullnessKind();
      PushSuperblock(superblocks_[sk][new_fk], sb);

      used_ -= sb->GetBlockSize();
    }

    void AcquireSuperblock(Superblock *sb) {
      sb->SetOwner(this);
      auto sk = sb->GetSizeKind();
      auto fk = sb->GetFullnessKind();
      PushSuperblock(superblocks_[sk][fk], sb);

      allocated_ += SBSize;
      used_ += sb->GetUsed();
    }

    Superblock * ReleaseSuperblock(size_t size_kind) {
      for (int fk = 0; fk < FullnessKinds + 1; ++fk) {
        if (superblocks_[size_kind][fk] != nullptr) {
            auto sb = PopSuperblock(superblocks_[size_kind][fk]);
            ReleaseSuperblock(sb);
            return sb;
        }
      }

      return nullptr;
    }

    Superblock * ReleaseSuperblock() {
      for (int fk = 0; fk < FullnessKinds + 1; ++fk) {
        for (auto &sbs : superblocks_) {
          if (sbs[fk] != nullptr) {
            auto sb = PopSuperblock(sbs[fk]);
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
    void CutSuperblock(Superblock *sb) {
      auto next = sb->GetNext();
      auto prev = sb->GetPrev();
      sb->SetNext(nullptr);
      sb->SetPrev(nullptr);

      if (next != nullptr) {
        next->SetPrev(prev);
      } 
      if (prev != nullptr) {
        prev->SetNext(next);
      }
    }

    Superblock * PopSuperblock(Superblock *&list) {
      auto sb = list;
      auto next = sb->GetNext();
      sb->SetNext(nullptr);

      if (next != nullptr) {
        next->SetPrev(nullptr);
      }
      list = next;

      return sb;
    }

    void ReleaseSuperblock(Superblock *sb) {
      allocated_ -= SBSize;
      used_ -= sb->GetUsed();
    }

    void PushSuperblock(Superblock *&list, Superblock *sb) {
      sb->SetNext(list);
      if (list != nullptr) {
        list->SetPrev(sb);
      }
      list = sb;
    }

    std::mutex lock_;

    std::vector<std::vector<Superblock *>> superblocks_;

    size_t allocated_;
    size_t used_;
  };

  size_t kReserveSize = 1;

  static void * Mmap(size_t size) {
    return std::malloc(size);
    /*size += sizeof(size_t);
    void *ptr = mmap(0, size, PROT_READ|PROT_WRITE,
                     MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    *((size_t *) ptr) = size;
    return &((size_t *) ptr)[1];*/
  }

  static void Munmap(void *ptr) {
    std::free(ptr);
    // munmap(&((size_t *) ptr)[-1], ((size_t *) ptr)[-1]);
  }

  Heap & GetHeap() {
    auto heap_id = hasher(std::this_thread::get_id()) % (heaps_.size() - 1);
    return heaps_[heap_id + 1];
  }

  void * AllocateBig(size_t size) {
    uint8_t *data = (uint8_t *) Mmap(size + sizeof(Superblock *));
    data += sizeof(Superblock *);

    if (data != nullptr) {
      SetBlockOwner(data, nullptr);
      return  data;
    } else {
      return nullptr;
    }
  }

  void FreeBig(void *data) {
    Munmap((uint8_t *) data - sizeof(Superblock *));
  }

  static void SetBlockOwner(void *block, Superblock *owner) {
    ((Superblock **) block)[-1] = owner;
  }

  static Superblock * GetBlockOwner(void *block) {
    return ((Superblock **) block)[-1];
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

static MtAllocator<1 << 14, 5> & GetAllocator() {
  static int heap_count = std::max(std::thread::hardware_concurrency(), 1U) * 2;
  static MtAllocator<1 << 14, 5> allocator(heap_count);
  return allocator;
}

void * mtalloc(size_t bytes) {
  return GetAllocator().Allocate(bytes);
}

void mtfree(void* ptr) {
  GetAllocator().Free(ptr);
}

void mtstats() {
  GetAllocator().stats();
}
