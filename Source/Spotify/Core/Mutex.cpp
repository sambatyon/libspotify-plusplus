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

#include "Spotify/Core/Mutex.h"

namespace Spotify { namespace Core 
{
	Mutex::Mutex( const char* name )
	{	
	}

	Mutex::~Mutex()
	{		
	}
	
	bool Mutex::TryLock()
	{
		return m_mutex.try_lock();
	}

	void Mutex::Lock()
	{
		m_mutex.lock();
	}

	void Mutex::Unlock()
	{
		m_mutex.unlock();
	}
		
	ScopedLock::ScopedLock( Mutex* pMutex )
	{
		m_pMutex = pMutex;
		m_pMutex->Lock();
	}

	ScopedLock::~ScopedLock()
	{
		m_pMutex->Unlock();
		m_pMutex = NULL;
	}

} // Core
} // Spotify