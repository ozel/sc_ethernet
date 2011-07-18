// Copyright (c) 2011, XMOS Ltd., All rights reserved
// This software is freely distributable under a derivative of the
// University of Illinois/NCSA Open Source License posted in
// LICENSE.txt and at <http://github.xcore.com/>

#include <mii_queue.h>
#include <mii.h>

#ifndef ETHERNET_USE_HARDWARE_LOCKS
#include "swlock.h"
#else
#include "hwlock.h"
#endif

extern mii_packet_t mii_packet_buf[];

#ifndef ETHERNET_USE_HARDWARE_LOCKS
swlock_t queue_locks[MAC_MAX_NUM_QUEUES];
swlock_t tc_lock = INITIAL_SWLOCK_VALUE;
#else
extern hwlock_t ethernet_memory_lock;
#endif

int get_and_dec_transmit_count(int buf0) 
{
  mii_packet_t *buf = (mii_packet_t *) buf0;
  int count;
#ifndef ETHERNET_USE_HARDWARE_LOCKS
  swlock_acquire(&tc_lock);
#else
  __hwlock_acquire(ethernet_memory_lock);
#endif
  count = buf->tcount;
  if (count) 
    buf->tcount = count - 1;
#ifndef ETHERNET_USE_HARDWARE_LOCKS
  swlock_release(&tc_lock);
#else
  __hwlock_release(ethernet_memory_lock);
#endif
  return count;
}

void incr_transmit_count(int buf0, int incr) 
{
  mii_packet_t *buf = (mii_packet_t *) buf0;
#ifndef ETHERNET_USE_HARDWARE_LOCKS
  swlock_acquire(&tc_lock);
#else
  __hwlock_acquire(ethernet_memory_lock);
#endif
  buf->tcount += incr;

#ifndef ETHERNET_USE_HARDWARE_LOCKS
  swlock_release(&tc_lock);
#else
  __hwlock_release(ethernet_memory_lock);
#endif
}

void init_queue(mii_queue_t *q)
{
#ifndef ETHERNET_USE_HARDWARE_LOCKS
  static int next_qlock = 1;
  q->lock = (int) &queue_locks[next_qlock];
  next_qlock++;
  swlock_init((swlock_t *) q->lock);
#endif

  q->rdIndex = 0;
  q->wrIndex = 0;
  return;
}

int get_queue_entry(mii_queue_t *q) 
{
  int i=0;
  int rdIndex, wrIndex;

#ifndef ETHERNET_USE_HARDWARE_LOCKS
  swlock_acquire((swlock_t *) q->lock);
#else
  __hwlock_acquire(ethernet_memory_lock);
#endif
  
  rdIndex = q->rdIndex;
  wrIndex = q->wrIndex;

  if (rdIndex == wrIndex)
    i = 0;
  else {
    i = q->fifo[rdIndex];
    rdIndex++;
    rdIndex *= (rdIndex != MAC_MAX_ENTRIES);
    q->rdIndex = rdIndex;
  }
#ifndef ETHERNET_USE_HARDWARE_LOCKS
  swlock_release((swlock_t *) q->lock);
#else
  __hwlock_release(ethernet_memory_lock);
#endif
  return i;
}

void add_queue_entry(mii_queue_t *q, int i) 
{
  int wrIndex;

#ifndef ETHERNET_USE_HARDWARE_LOCKS
  swlock_acquire((swlock_t *) q->lock);
#else
  __hwlock_acquire(ethernet_memory_lock);
#endif

  wrIndex = q->wrIndex;
  q->fifo[wrIndex] = i;
  wrIndex++;
  wrIndex *= (wrIndex != MAC_MAX_ENTRIES);
  q->wrIndex = wrIndex;

#ifndef ETHERNET_USE_HARDWARE_LOCKS
  swlock_release((swlock_t *) q->lock);
#else
  __hwlock_release(ethernet_memory_lock);
#endif
  return;
}

