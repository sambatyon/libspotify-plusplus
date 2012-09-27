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

// c-lib includes
#include <assert.h>

// local includes
#include "spotify/PlayListContainer.hpp"
#include "spotify/PlayListFolder.hpp"
#include "spotify/Session.hpp"

// debugging
#include "debug/Debug.hpp"
#define LOG( msg, ... ) //Debug::PrintLine( msg, __VA_ARGS__ );

#include <cstdlib>

namespace spotify {
PlayListContainer::PlayListContainer(boost::shared_ptr<Session> session) : PlayListElement(session), container_(NULL)
                                                                         , loading_(false), playlists_() {
}

PlayListContainer::~PlayListContainer() {
    Unload();
}

PlayListElement::PlayListType PlayListContainer::GetType() {
    return PLAYLIST_CONTAINER;
}

void PlayListContainer::GetCallbacks(sp_playlistcontainer_callbacks &callbacks) {
    std::memset(&callbacks, 0, sizeof(callbacks));

    callbacks.container_loaded = callback_container_loaded;
    callbacks.playlist_added = callback_playlist_added;
    callbacks.playlist_moved = callback_playlist_moved;
    callbacks.playlist_removed = callback_playlist_removed;
}

bool PlayListContainer::Load(sp_playlistcontainer *container) {
    container_ = container;
    sp_playlistcontainer_callbacks callbacks;
    GetCallbacks(callbacks);
    sp_playlistcontainer_add_callbacks(container_, &callbacks, this);
    loading_ = true;
    return true;
}

void PlayListContainer::Unload() {
    if (container_) {
        sp_playlistcontainer_callbacks callbacks;
        GetCallbacks(callbacks);
        sp_playlistcontainer_remove_callbacks(container_, &callbacks, this);
        container_ = NULL;
        playlists_.clear();
        loading_ = false;
    }
}

bool PlayListContainer::IsLoading(bool recursive) {
    if (loading_)
        return true;

    if (recursive) {
        int numPlayLists = GetNumChildren();

        for (int i = 0; i < numPlayLists; i++) {
            if (playlists_[i]->IsLoading(recursive))
                return true;
        }
    }

    return false;
}

void PlayListContainer::AddPlayList(boost::shared_ptr<PlayListElement> playlist) {
    playlist->SetParent(shared_from_this());
    playlists_.push_back(playlist);
}

bool PlayListContainer::HasChildren() {
    return !playlists_.empty();
}

int PlayListContainer::GetNumChildren() {
    return playlists_.size();
}

boost::shared_ptr<PlayListElement> PlayListContainer::GetChild(int index) {
    return playlists_[index];
}

void PlayListContainer::DumpToTTY(int level) {
    debug::PrintLine(level, "PlayListContainer");

    level++;

    int numPlayLists = GetNumChildren();

    for (int i = 0; i < numPlayLists; i++)
        GetChild(i)->DumpToTTY(level);
}

std::string PlayListContainer::GetName() {
    return "Container";
}

PlayListContainer *PlayListContainer::GetPlayListContainer(sp_playlistcontainer *pc, void *userdata) {
    PlayListContainer *container = reinterpret_cast<PlayListContainer *>(userdata);
    BOOST_ASSERT(container->container_ == pc);

    return container;
}

void PlayListContainer::callback_playlist_added(sp_playlistcontainer *pc, sp_playlist *playlist, int position, 
                                                void *userdata) {
    PlayListContainer *container = GetPlayListContainer(pc, userdata);
    container->OnPlaylistAdded(playlist, position);
}

void PlayListContainer::callback_playlist_removed(sp_playlistcontainer *pc, sp_playlist *playlist, int position, 
                                                  void *userdata) {
    PlayListContainer *container = GetPlayListContainer(pc, userdata);
    container->OnPlaylistRemoved(playlist, position);
}

void PlayListContainer::callback_playlist_moved(sp_playlistcontainer *pc, sp_playlist *playlist, int position, 
                                                int new_position, void *userdata) {
    PlayListContainer *container = GetPlayListContainer(pc, userdata);
    container->OnPlaylistMoved(playlist, position, new_position);
}

void PlayListContainer::callback_container_loaded(sp_playlistcontainer *pc, void *userdata) {
    PlayListContainer *container = GetPlayListContainer(pc, userdata);
    container->loading_ = false;
    container->OnContainerLoaded();
}

void PlayListContainer::OnPlaylistAdded(sp_playlist *playlist, int position) {
    LOG("OnPlaylistAdded [0x%08X]", playlist);
}

void PlayListContainer::OnPlaylistRemoved(sp_playlist *playlist, int position) {
    LOG("OnPlaylistRemoved [0x%08X]", playlist);
}

void PlayListContainer::OnPlaylistMoved(sp_playlist *playlist, int position, int new_position) {
    LOG("OnPlaylistMoved [0x%08X]", playlist);
}

void PlayListContainer::OnContainerLoaded() {
    LOG("OnContainerLoaded");

    int num_playlists = sp_playlistcontainer_num_playlists(container_);

    boost::shared_ptr<PlayListElement> it_container = shared_from_this();

    for (int i = 0; i < num_playlists; i++) {
        sp_playlist_type type = sp_playlistcontainer_playlist_type(container_, i);

        switch (type) {
            case SP_PLAYLIST_TYPE_PLAYLIST: {
                sp_playlist *p = sp_playlistcontainer_playlist(container_, i);

                boost::shared_ptr<PlayList> playlist = session_->CreatePlayList();
                playlist->Load(p);

                it_container->AddPlayList(playlist);
            }   break;
            case SP_PLAYLIST_TYPE_START_FOLDER: {
                boost::shared_ptr<PlayListFolder> folder = session_->CreatePlayListFolder();
                folder->Load(container_, i);

                it_container->AddPlayList(folder);
                it_container = folder;
            }   break;
            case SP_PLAYLIST_TYPE_END_FOLDER:
                it_container = it_container->GetParent();
                break;
            case SP_PLAYLIST_TYPE_PLACEHOLDER:
            default:
                LOG("OTHER???");
                // ??
                break;
        }

    }

    BOOST_ASSERT(it_container == shared_from_this());
}
}
