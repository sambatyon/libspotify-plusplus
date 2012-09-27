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

#include "spotify/PlayListElement.hpp"
#include "spotify/Session.hpp"


namespace spotify {
PlayListElement::PlayListElement(boost::shared_ptr<Session> session) : session_(session), user_data_(NULL) {
}

PlayListElement::~PlayListElement() {
}

boost::shared_ptr<PlayListElement> PlayListElement::GetParent() const {
    return parent_.lock();
}

void PlayListElement::SetParent(boost::shared_ptr<PlayListElement> parent) {
    parent_ = parent;
}

void *PlayListElement::GetUserData() {
    return user_data_;
}

void PlayListElement::SetUserData(void *user_data) {
    user_data_ = user_data;
}

boost::shared_ptr<Session> PlayListElement::GetSession() {
    return session_;
}
} // spotify
