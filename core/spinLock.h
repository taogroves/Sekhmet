//
// Created by Tao Groves on 2/24/23.
//

#ifndef SEKHMET_SPINLOCK_H
#define SEKHMET_SPINLOCK_H

#endif //SEKHMET_SPINLOCK_H

#include <atomic>

// spinlock class
class spinLock {
private:
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
public:


    void lock() {
        while (flag.test_and_set(std::memory_order_acquire));
    }

    void unlock() {
        flag.clear(std::memory_order_release);
    }
};