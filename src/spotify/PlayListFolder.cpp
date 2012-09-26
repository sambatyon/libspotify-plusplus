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

// Lib Includes
#include "spotify/PlayListFolder.hpp"

// Local Includes
#include "Debug/Debug.hpp"

namespace spotify {
PlayListFolder::PlayListFolder(boost::shared_ptr<Session> session) : PlayListElement(session), pContainer_(NULL), containerIndex_(-1) {
}

PlayListFolder::~PlayListFolder() {
    Unload();
}

bool PlayListFolder::IsLoading(bool recursive) {
    int numPlayLists = playLists_.size();

    if (!pContainer_) {
        return false;
    }

    if (recursive) {
        for (int i = 0; i < numPlayLists; i++) {
            if (playLists_[i]->IsLoading(recursive)) {
                return true;
            }
        }
    }

    return false;
}

PlayListElement::eType PlayListFolder::GetType() {
    return PLAYLIST_FOLDER;
}

bool PlayListFolder::Load(sp_playlistcontainer *container, int index) {
    pContainer_ = container;
    containerIndex_ = index;

    return true;
}

void PlayListFolder::Unload() {
    pContainer_ = NULL;
    containerIndex_ = -1;
}

void PlayListFolder::AddPlayList(boost::shared_ptr<PlayListElement> playList) {
    playList->SetParent(shared_from_this());
    playLists_.push_back(playList);
}

std::string PlayListFolder::GetName() {
    const int BUFFER_SIZE = 256;
    char buffer[BUFFER_SIZE];
    sp_playlistcontainer_playlist_folder_name(pContainer_, containerIndex_, buffer, BUFFER_SIZE);

    std::string folderName = buffer;
    return folderName;
}

sp_uint64 PlayListFolder::GetGroupID() {
    sp_uint64 groupID = sp_playlistcontainer_playlist_folder_id(pContainer_, containerIndex_);
    return groupID;
}

bool PlayListFolder::HasChildren() {
    return !playLists_.empty();
}

int PlayListFolder::GetNumChildren() {
    return playLists_.size();
}

boost::shared_ptr<PlayListElement> PlayListFolder::GetChild(int index) {
    return playLists_[index];
}

void PlayListFolder::DumpToTTY(int level) {
    Debug::PrintLine(level, "Folder [%s]", GetName().c_str());

    level ++;

    int numPlayLists = GetNumChildren();

    for (int i = 0; i < numPlayLists; i++) {
        GetChild(i)->DumpToTTY(level);
    }
}
}