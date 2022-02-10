/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2022 Filipe Coelho <falktx@falktx.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * For a full copy of the GNU General Public License see the LICENSE file.
 */

#pragma once

#include <pthread.h>

#ifdef __MOD_DEVICES__
#include <linux/futex.h>
#include <sys/time.h>
#include <errno.h>
#include <syscall.h>
#include <unistd.h>
#endif

/* replace Rack's mutex with our own custom one, which can do priority inversion. */

namespace rack {


struct SharedMutex {
    pthread_mutex_t readLock;
#ifdef __MOD_DEVICES__
    int writeLock;
#else
    pthread_mutex_t writeLock;
#endif

    SharedMutex() noexcept {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&readLock, &attr);
        pthread_mutexattr_destroy(&attr);

#ifdef __MOD_DEVICES__
        writeLock = 1;
#else
        pthread_mutexattr_t attr2;
        pthread_mutexattr_init(&attr2);
        pthread_mutexattr_setprotocol(&attr2, PTHREAD_PRIO_NONE);
        pthread_mutexattr_settype(&attr2, PTHREAD_MUTEX_NORMAL);
        pthread_mutex_init(&writeLock, &attr2);
        pthread_mutexattr_destroy(&attr2);
#endif
    }

    ~SharedMutex() noexcept {
        pthread_mutex_destroy(&readLock);
#ifndef __MOD_DEVICES__
        pthread_mutex_destroy(&writeLock);
#endif
    }

    // for std::lock_guard usage, writers lock
    void lock() noexcept {
#ifdef __MOD_DEVICES__
        for (;;)
        {
            if (__sync_bool_compare_and_swap(&writeLock, 1, 0))
                return;

            if (syscall(__NR_futex, &writeLock, FUTEX_WAIT_PRIVATE, 0, nullptr, nullptr, 0) != 0)
            {
                if (errno != EAGAIN && errno != EINTR)
                    return;
            }
        }
#else
        pthread_mutex_lock(&writeLock);
#endif
        pthread_mutex_lock(&readLock);
    }
    void unlock() noexcept {
        pthread_mutex_unlock(&readLock);
#ifdef __MOD_DEVICES__
        if (__sync_bool_compare_and_swap(&writeLock, 0, 1))
            syscall(__NR_futex, &writeLock, FUTEX_WAKE_PRIVATE, 1, nullptr, nullptr, 0);
#else
        pthread_mutex_unlock(&writeLock);
#endif
    }

    // for SharedLock usage, readers lock
    void lock_shared() noexcept {
        pthread_mutex_lock(&readLock);
    }
    void unlock_shared() noexcept {
        pthread_mutex_unlock(&readLock);
    }
};


template <class Mutex>
struct SharedLock {
    Mutex& mutex;

    SharedLock(Mutex& m) noexcept : mutex(m) {
        mutex.lock_shared();
    }
    ~SharedLock() noexcept {
        mutex.unlock_shared();
    }
};


} // namespace rack
