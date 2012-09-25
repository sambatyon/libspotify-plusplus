/*
 * Copyright 2011 Jim Knowler
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

#pragma once

#include <boost/thread/mutex.hpp>

namespace Spotify {
namespace Core {
class Mutex {
public:

    Mutex(const char *name = "");
    virtual ~Mutex();

    bool		TryLock();
    void		Lock();
    void		Unlock();

private:
    boost::mutex	m_mutex;
};

class ScopedLock {
public:
    ScopedLock(Mutex *pMutex);
    virtual ~ScopedLock();
private:
    Mutex		*m_pMutex;
};

}
}