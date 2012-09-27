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

// Lib Includes
#include "spotify/PlayListFolder.hpp"

#include <boost/format.hpp>

#include <log4cplus/loggingmacros.h>
#include <log4cplus/logger.h>


namespace spotify {
namespace {
log4cplus::Logger logger = log4cplus::Logger::getInstance("spotify.PlayListFolder");
}

PlayListFolder::PlayListFolder(boost::shared_ptr<Session> session) : PlayListElement(session), playlists_()
                                                                   , container_(NULL), container_index_(-1) {
}

PlayListFolder::~PlayListFolder() {
    Unload();
}

bool PlayListFolder::IsLoading(bool recursive) {
    int numPlayLists = playlists_.size();

    if (!container_)
        return false;

    if (recursive) {
        for (int i = 0; i < numPlayLists; i++) {
            if (playlists_[i]->IsLoading(recursive))
                return true;
        }
    }

    return false;
}

PlayListElement::PlayListType PlayListFolder::GetType() {
    return PLAYLIST_FOLDER;
}

bool PlayListFolder::Load(sp_playlistcontainer *container, int index) {
    container_ = container;
    container_index_ = index;

    return true;
}

void PlayListFolder::Unload() {
    container_ = NULL;
    container_index_ = -1;
}

void PlayListFolder::AddPlayList(boost::shared_ptr<PlayListElement> playList) {
    playList->SetParent(shared_from_this());
    playlists_.push_back(playList);
}

std::string PlayListFolder::GetName() {
    const int BUFFER_SIZE = 256;
    char buffer[BUFFER_SIZE];
    sp_playlistcontainer_playlist_folder_name(container_, container_index_, buffer, BUFFER_SIZE);

    std::string folderName = buffer;
    return folderName;
}

sp_uint64 PlayListFolder::GetGroupID() {
    sp_uint64 groupID = sp_playlistcontainer_playlist_folder_id(container_, container_index_);
    return groupID;
}

bool PlayListFolder::HasChildren() {
    return !playlists_.empty();
}

int PlayListFolder::GetNumChildren() {
    return playlists_.size();
}

boost::shared_ptr<PlayListElement> PlayListFolder::GetChild(int index) {
    return playlists_[index];
}

void PlayListFolder::DumpToTTY(int level) {
    std::string indent = "";
    for (int i = 0; i < level; ++i)
        indent += " ";
    LOG4CPLUS_DEBUG(logger, (boost::format("Folder [%s]") % GetName()));

    ++level;

    int numPlayLists = GetNumChildren();

    for (int i = 0; i < numPlayLists; i++)
        GetChild(i)->DumpToTTY(level);
}
}
