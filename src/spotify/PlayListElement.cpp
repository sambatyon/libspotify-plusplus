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

#include "spotify/PlayListElement.hpp"
#include "spotify/Session.hpp"


namespace spotify {
PlayListElement::PlayListElement(boost::shared_ptr<Session> session) : m_session(session), m_pUserData(NULL) {
}

PlayListElement::~PlayListElement() {
}

boost::shared_ptr<PlayListElement> PlayListElement::GetParent() const {
    return m_parent.lock();
}

void PlayListElement::SetParent(boost::shared_ptr<PlayListElement> parent) {
    m_parent = parent;
}

void *PlayListElement::GetUserData() {
    return m_pUserData;
}

void PlayListElement::SetUserData(void *pUserData) {
    m_pUserData = pUserData;
}

boost::shared_ptr<Session> PlayListElement::GetSession() {
    return m_session;
}

} // spotify
