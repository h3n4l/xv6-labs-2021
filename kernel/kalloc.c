// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.
struct ref_count{
  int count;
  struct spinlock lock;
};

struct ref_count ref_count[PHYNPG];


struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

void
kinit()
{
  uint64 i;
  for(i = 0; i < PHYNPG; i++){
    initlock(&(ref_count[i].lock), "ref_count");
    ref_count[i].count = 1;
  }
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");
  if(get_refcount((uint64)pa) <= 0)
    panic("kfree of ref_count");
  // Be careful about the kinit_freerange_end, only handle the ref_count after kinit!!!
    sub_refcount((uint64)pa);
  if(get_refcount((uint64)pa) == 0){
    // Fill with junk to catch dangling refs.
    memset(pa, 1, PGSIZE);

    r = (struct run*)pa;

    acquire(&kmem.lock);
    r->next = kmem.freelist;
    kmem.freelist = r;
    release(&kmem.lock);
  }
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r){
    memset((char*)r, 5, PGSIZE); // fill with junk
    // Add ref_count
    add_refcount((uint64)r);
  }
  return (void*)r;
}

inline uint64 RCIDX(uint64 pa){
  uint64 idx = (PGROUNDDOWN(pa) - KERNBASE) / PGSIZE;
  if(idx >= PHYNPG)
    panic("PCIDX");
  return idx;
}

inline void add_refcount(uint64 pa){
  uint64 idx = RCIDX((uint64)pa);
  acquire(&(ref_count[idx].lock));
  ref_count[RCIDX((uint64)pa)].count += 1;
  release(&(ref_count[idx].lock));
}

inline void sub_refcount(uint64 pa){
  uint64 idx = RCIDX((uint64)pa);
  acquire(&(ref_count[idx].lock));
  ref_count[RCIDX((uint64)pa)].count -=1;
  release(&(ref_count[idx].lock));
}

inline int get_refcount(uint64 pa){
  uint64 idx = RCIDX((uint64)pa);
  int count;
  acquire(&(ref_count[idx].lock));
  count = ref_count[idx].count;
  release(&(ref_count[idx].lock));
  return count;
}