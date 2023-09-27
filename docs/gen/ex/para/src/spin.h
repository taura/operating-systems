#include <assert.h>
#include <errno.h>

typedef struct {
  volatile int locked;
} spinlock_t;

int spin_init(spinlock_t *lock, int pshared) {
  (void)pshared;
  lock->locked = 0;
  return 0;
}

int spin_lock(spinlock_t *lock) {
  volatile int * x = &lock->locked;
  while (1) {
    while (*x) { }
    if (__sync_bool_compare_and_swap(x, 0, 1)) {
      __sync_synchronize();
      return 0;
    }
  }
}

int spin_unlock(spinlock_t *lock) {
  assert(lock->locked);
  __sync_synchronize();
  lock->locked = 0;
  return 0;
}

int spin_trylock(spinlock_t *lock) {
  volatile int * x = &lock->locked;
  if (__sync_bool_compare_and_swap(x, 0, 1)) {
    __sync_synchronize();
    return 0;
  }
  else return EBUSY;
}

