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

/* replace Rack's mutex with our own custom one, which can do priority inversion. */

namespace rack {


struct SharedMutex {
    pthread_mutex_t readLock, writeLock;

    SharedMutex() noexcept {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&readLock, &attr);
        pthread_mutexattr_destroy(&attr);

        pthread_mutexattr_t attr2;
        pthread_mutexattr_init(&attr2);
        pthread_mutexattr_setprotocol(&attr2, PTHREAD_PRIO_NONE);
        pthread_mutexattr_settype(&attr2, PTHREAD_MUTEX_NORMAL);
        pthread_mutex_init(&writeLock, &attr2);
        pthread_mutexattr_destroy(&attr2);
    }

    ~SharedMutex() noexcept {
        pthread_mutex_destroy(&readLock);
        pthread_mutex_destroy(&writeLock);
    }

    // for std::lock_guard usage, writers lock
    void lock() noexcept {
        pthread_mutex_lock(&writeLock);
        pthread_mutex_lock(&readLock);
    }
    void unlock() noexcept {
        pthread_mutex_unlock(&readLock);
        pthread_mutex_unlock(&writeLock);
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
