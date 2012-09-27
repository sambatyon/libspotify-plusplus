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

// std includes
#include <assert.h>

// local includes
#include "spotify/PlayList.hpp"
#include "spotify/Session.hpp"
#include "spotify/Track.hpp"

// Debugging
#include "debug/Debug.hpp"
#define LOG( msg, ... ) //Debug::PrintLine( msg, __VA_ARGS__ )

namespace spotify {
PlayList::PlayList(boost::shared_ptr<Session> session) : PlayListElement(session), pPlayList_(NULL), isLoading_(false) {
}

PlayList::~PlayList() {
    Unload();
}

PlayListElement::eType PlayList::GetType() {
    return PLAYLIST;
}

bool PlayList::Load(sp_playlist *playlist) {
    pPlayList_ = playlist;
    sp_playlist_add_ref(playlist);

    sp_playlist_callbacks callbacks;
    GetCallbacks(callbacks);

    sp_playlist_add_callbacks(pPlayList_, &callbacks, this);

    if (!sp_playlist_is_loaded(pPlayList_)) {
        isLoading_ = true;
    } else {
        LoadTracks();
    }

    return true;
}

void PlayList::LoadTracks() {
    int numTracks = sp_playlist_num_tracks(pPlayList_);

    tracks_.empty();
    tracks_.reserve(numTracks);

    for (int j = 0; j < numTracks; j++) {
        sp_track *t = sp_playlist_track(pPlayList_, j);

        boost::shared_ptr<Track> track = session_->CreateTrack();
        track->Load(t);

        tracks_.push_back(track);
    }
}

void PlayList::Unload() {
    if (pPlayList_) {
        sp_playlist_callbacks callbacks;
        GetCallbacks(callbacks);

        sp_playlist_remove_callbacks(pPlayList_, &callbacks, this);

        sp_playlist_release(pPlayList_);
        tracks_.clear();

        isLoading_ = false;
        pPlayList_ = NULL;
    }
}

bool PlayList::IsLoading(bool recursive) {
    //bool isLoaded = sp_playlist_is_loaded( pPlayList_ );


    int numTracks = GetNumTracks();

    // is playlist, or any of its' tracks loading?

    if (isLoading_) {
        return true;
    }

    if (recursive) {
        for (int i = 0; i < numTracks; i++) {
            if (tracks_[i]->IsLoading(recursive)) {
                return true;
            }
        }
    }

    return false;
}

int PlayList::GetNumTracks() {
    return tracks_.size();
}

boost::shared_ptr<Track> PlayList::GetTrack(int index) {
    return tracks_[ index ];
}

std::string PlayList::GetName() {
    const char *name = sp_playlist_name(pPlayList_);
    return name;
}

bool PlayList::HasChildren() {
    return !tracks_.empty();
}

int PlayList::GetNumChildren() {
    return tracks_.size();
}

boost::shared_ptr<PlayListElement> PlayList::GetChild(int index) {
    return tracks_[index];
}

void PlayList::DumpToTTY(int level) {
    debug::PrintLine(level, "PlayList [%s]", GetName().c_str());

    level ++;

    int numTracks = GetNumTracks();

    for (int i = 0; i < numTracks; i++) {
        GetTrack(i)->DumpToTTY(level);
    }
}

void PlayList::GetCallbacks(sp_playlist_callbacks &callbacks) {
    memset(&callbacks, 0, sizeof(callbacks));

    callbacks.description_changed = callback_description_changed;
    callbacks.image_changed = callback_image_changed;
    callbacks.playlist_metadata_updated = callback_playlist_metadata_updated;
    callbacks.playlist_renamed = callback_playlist_renamed;
    callbacks.playlist_state_changed = callback_playlist_state_changed;
    callbacks.playlist_update_in_progress = callback_playlist_update_in_progress;
    callbacks.tracks_added = callback_tracks_added;
    callbacks.tracks_moved = callback_tracks_moved;
    callbacks.tracks_removed = callback_tracks_removed;
    callbacks.track_created_changed = callback_track_created_changed;
    callbacks.track_seen_changed = callback_track_seen_changed;
}

PlayList *PlayList::GetPlayListFromUserData(sp_playlist *pl, void *userdata) {
    PlayList *pPlayList = reinterpret_cast<PlayList *>(userdata);
    assert(pPlayList->pPlayList_ == pl);

    return pPlayList;
}

void PlayList::callback_tracks_added(sp_playlist *pl, sp_track *const *tracks, int num_tracks, int position, void *userdata) {
    PlayList *pPlayList = GetPlayListFromUserData(pl, userdata);
    pPlayList->OnTracksAdded(tracks, num_tracks, position);
}

void PlayList::callback_tracks_removed(sp_playlist *pl, const int *tracks, int num_tracks, void *userdata) {
    PlayList *pPlayList = GetPlayListFromUserData(pl, userdata);
    pPlayList->OnTracksRemoved(tracks, num_tracks);
}

void PlayList::callback_tracks_moved(sp_playlist *pl, const int *tracks, int num_tracks, int new_position, void *userdata) {
    PlayList *pPlayList = GetPlayListFromUserData(pl, userdata);
    pPlayList->OnTracksMoved(tracks, num_tracks, new_position);
}

void PlayList::callback_playlist_renamed(sp_playlist *pl, void *userdata) {
    PlayList *pPlayList = GetPlayListFromUserData(pl, userdata);
    pPlayList->OnPlaylistRenamed();
}

void PlayList::callback_playlist_state_changed(sp_playlist *pl, void *userdata) {
    PlayList *pPlayList = GetPlayListFromUserData(pl, userdata);
    pPlayList->OnPlaylistStateChanged();
}

void PlayList::callback_playlist_update_in_progress(sp_playlist *pl, bool done, void *userdata) {
    PlayList *pPlayList = GetPlayListFromUserData(pl, userdata);
    pPlayList->OnPlaylistUpdateInProgress(done);
}

void PlayList::callback_playlist_metadata_updated(sp_playlist *pl, void *userdata) {
    PlayList *pPlayList = GetPlayListFromUserData(pl, userdata);
    pPlayList->OnPlaylistMetadataUpdated();
}

void PlayList::callback_track_created_changed(sp_playlist *pl, int position, sp_user *user, int when, void *userdata) {
    PlayList *pPlayList = GetPlayListFromUserData(pl, userdata);
    pPlayList->OnTrackCreatedChanged(position, user, when);
}

void PlayList::callback_track_seen_changed(sp_playlist *pl, int position, bool seen, void *userdata) {
    PlayList *pPlayList = GetPlayListFromUserData(pl, userdata);
    pPlayList->OnTrackSeenChanged(position, seen);
}

void PlayList::callback_description_changed(sp_playlist *pl, const char *desc, void *userdata) {
    PlayList *pPlayList = GetPlayListFromUserData(pl, userdata);
    pPlayList->OnDescriptionChanged(desc);
}

void PlayList::callback_image_changed(sp_playlist *pl, const byte *image, void *userdata) {
    PlayList *pPlayList = GetPlayListFromUserData(pl, userdata);
    pPlayList->OnImageChanged(image);
}

void PlayList::OnTracksAdded(sp_track *const *tracks, int numTracks, int position) {
    LOG("PlayList::OnTracksAdded [0x%08X] numTracks[%d] position[%d]", this, numTracks, position);
}

void PlayList::OnTracksRemoved(const int *tracks, int numTracks) {
    LOG("PlayList::OnTracksRemoved [0x%08X] numTracks[%d]", this, numTracks);
}

void PlayList::OnTracksMoved(const int *tracks, int numTracks, int newPosition) {
    LOG("PlayList::OnTracksMoved [0x%08X] numTracks[%d] newPosition[%d]", this, numTracks, newPosition);
}

void PlayList::OnPlaylistRenamed() {
    LOG("PlayList::OnPlaylistRenamed [0x%08X]", this);
}

void PlayList::OnPlaylistStateChanged() {
    bool isLoaded = sp_playlist_is_loaded(pPlayList_);

    LOG("PlayList::OnPlaylistStateChanged [0x%08X] m_isLoading [%d] isLoaded [%d]", this, isLoading_, isLoaded);

    if (isLoading_ && isLoaded) {
        isLoading_ = false;

        LoadTracks();
    }
}

void PlayList::OnPlaylistUpdateInProgress(bool done) {
    LOG("PlayList::OnPlaylistUpdateInProgress [0x%08X] - done [%d]", this, done);
}

void PlayList::OnPlaylistMetadataUpdated() {
    LOG("PlayList::OnPlaylistMetadataUpdated [0x%08X]", this);
}

void PlayList::OnTrackCreatedChanged(int position, sp_user *user, int when) {
    LOG("PlayList::OnTrackCreatedChanged [0x%08X]", this);
}

void PlayList::OnTrackSeenChanged(int position, bool seen) {
    LOG("PlayList::OnTrackSeenChanged [0x%08X]", this);
}

void PlayList::OnDescriptionChanged(const char *desc) {
    LOG("PlayList::OnDescriptionChanged [0x%08X]", this);
}

void PlayList::OnImageChanged(const byte *image) {
    LOG("PlayList::OnImageChanged [0x%08X]", this);
}



} // Spotify

