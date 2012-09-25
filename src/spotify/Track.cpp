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

// lib includes
#include "spotify/Track.hpp"
#include "spotify/Session.hpp"

// Local includes
#include "Debug/Debug.hpp"

namespace spotify {

Track::Track(boost::shared_ptr<Session> session) : PlayListElement(session) {
}

Track::~Track() {
    Unload();
}

bool Track::Load(sp_track *track) {
    pTrack_ = track;
    sp_track_add_ref(pTrack_);
    return true;
}

void Track::Unload() {
    sp_track_release(pTrack_);
    pTrack_ = NULL;
}

bool Track::IsLoading(bool recursive) {
    if (!sp_track_is_loaded(pTrack_))
        return true;

    return false;
}

std::string Track::GetName() {
    const char *name = sp_track_name(pTrack_);
    return name;
}

int Track::GetDuration() {
    int duration = sp_track_duration(pTrack_);
    return duration;
}

PlayListElement::eType Track::GetType() {
    return TRACK;
}

bool Track::HasChildren() {
    return false;
}

int Track::GetNumChildren() {
    return 0;
}

boost::shared_ptr<PlayListElement> Track::GetChild(int index) {
    return boost::shared_ptr<PlayListElement>();
}

void Track::DumpToTTY(int level) {
    int seconds = GetDuration() / 1000;
    int mins = seconds / 60;
    seconds %= 60;
    Debug::PrintLine(level, "Track [%s] [%d]mins [%d]secs", GetName().c_str(), mins, seconds);
}

int Track::GetNumArtists() {
    return sp_track_num_artists(pTrack_);
}

boost::shared_ptr<Artist> Track::GetArtist(int index) {
    boost::shared_ptr<Artist> artist = session_->CreateArtist();
    artist->Load(sp_track_artist(pTrack_, index));

    return artist;
}

boost::shared_ptr<Album> Track::GetAlbum() {
    boost::shared_ptr<Album> album = session_->CreateAlbum();
    album->Load(sp_track_album(pTrack_));

    return album;
}

int Track::GetDisc() {
    return sp_track_disc(pTrack_);
}

int Track::GetPopularity() {
    return sp_track_popularity(pTrack_);
}

bool Track::IsStarred() {
    return sp_track_is_starred(m_session->m_pSession, pTrack_);
}

void Track::SetStarred(bool isStarred) {
    sp_track_set_starred(m_session->m_pSession, &pTrack_, 1, isStarred);
}
}
