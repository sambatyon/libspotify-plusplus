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

// lib includes
#include "spotify/Session.hpp"
#include "spotify/Track.hpp"

#include <boost/format.hpp>

#include <log4cplus/loggingmacros.h>
#include <log4cplus/logger.h>

namespace spotify {
namespace {
log4cplus::Logger logger = log4cplus::Logger::getInstance("spotify.Track");
}

Track::Track(boost::shared_ptr<Session> session) : PlayListElement(session), track_(NULL) {
}

Track::~Track() {
    Unload();
}

bool Track::Load(sp_track *track) {
    track_ = track;
    sp_track_add_ref(track_);
    return true;
}

void Track::Unload() {
    sp_track_release(track_);
    track_ = NULL;
}

bool Track::IsLoading(bool recursive) {
    if (!sp_track_is_loaded(track_))
        return true;

    return false;
}

std::string Track::GetName() {
    const char *name = sp_track_name(track_);
    return name;
}

int Track::GetDuration() {
    int duration = sp_track_duration(track_);
    return duration;
}

PlayListElement::PlayListType Track::GetType() {
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
    std::string indent = "";
    for (int i = 0; i < level; ++i)
        indent += " ";
    LOG4CPLUS_DEBUG(logger, (boost::format("Track [%s] [%d]mins [%d]secs") % GetName() % mins % seconds));

    ++level;
}

int Track::GetNumArtists() {
    return sp_track_num_artists(track_);
}

boost::shared_ptr<Artist> Track::GetArtist(int index) {
    boost::shared_ptr<Artist> artist = session_->CreateArtist();
    artist->Load(sp_track_artist(track_, index));

    return artist;
}

boost::shared_ptr<Album> Track::GetAlbum() {
    boost::shared_ptr<Album> album = session_->CreateAlbum();
    album->Load(sp_track_album(track_));

    return album;
}

int Track::GetDisc() {
    return sp_track_disc(track_);
}

int Track::GetPopularity() {
    return sp_track_popularity(track_);
}

bool Track::IsStarred() {
    return sp_track_is_starred(session_->session_, track_);
}

void Track::SetStarred(bool isStarred) {
    sp_track_set_starred(session_->session_, &track_, 1, isStarred);
}
}
