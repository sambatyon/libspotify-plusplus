/*
 * Copyright 2011 Jim Knowler
 *           2012 Alexander Rojas
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "spotify/Core/Mutex.hpp"

namespace spotify {
namespace core {
Mutex::Mutex(const char *name) {
}

Mutex::~Mutex() {
}

bool Mutex::TryLock() {
    return mutex_.try_lock();
}

void Mutex::Lock() {
    mutex_.lock();
}

void Mutex::Unlock() {
    mutex_.unlock();
}

ScopedLock::ScopedLock(Mutex *mutex) {
    mutex_ = mutex;
    mutex_->Lock();
}

ScopedLock::~ScopedLock() {
    mutex_->Unlock();
    mutex_ = NULL;
}
} // Core
} // Spotify
