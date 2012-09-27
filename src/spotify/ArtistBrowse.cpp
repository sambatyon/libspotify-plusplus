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

// local includes
#include "spotify/ArtistBrowse.hpp"
#include "spotify/Session.hpp"
#include "spotify/Artist.hpp"

#include <log4cplus/logger.h>

namespace spotify {
namespace {
log4cplus::Logger logger = log4cplus::Logger::getInstance("spotify.ArtistBrowse");
}

ArtistBrowse::ArtistBrowse(boost::shared_ptr<Session> session, boost::shared_ptr<Artist> artist) 
    : session_(session) , artist_(artist) , artist_browse_(NULL) {
    artist_browse_ = sp_artistbrowse_create(session->session_, artist->artist_, SP_ARTISTBROWSE_FULL, callback_artistbrowse_complete, this);
}

ArtistBrowse::~ArtistBrowse() {
    sp_artistbrowse_release(artist_browse_);
}

bool ArtistBrowse::IsLoading() {
    return !sp_artistbrowse_is_loaded(artist_browse_);
}

boost::shared_ptr<Artist> ArtistBrowse::GetArtist() {
    sp_artist *sp_artist = sp_artistbrowse_artist(artist_browse_);

    boost::shared_ptr<Artist> artist = session_->CreateArtist();
    artist->Load(sp_artist);

    return artist;
}

int ArtistBrowse::GetNumPortraits() {
    return sp_artistbrowse_num_portraits(artist_browse_);
}

boost::shared_ptr<Image> ArtistBrowse::GetPortrait(int index) {
    boost::shared_ptr<Image> image = session_->CreateImage();

    image->Load(sp_artistbrowse_portrait(artist_browse_, index));

    return image;
}

int ArtistBrowse::GetNumTracks() {
    return sp_artistbrowse_num_tracks(artist_browse_);
}

boost::shared_ptr<Track> ArtistBrowse::GetTrack(int index) {
    boost::shared_ptr<Track> track = session_->CreateTrack();

    sp_track *sp_track = sp_artistbrowse_track(artist_browse_, index);

    track->Load(sp_track);

    return track;
}

int ArtistBrowse::GetNumAlbums() {
    return sp_artistbrowse_num_albums(artist_browse_);
}

boost::shared_ptr<Album> ArtistBrowse::GetAlbum(int index) {
    boost::shared_ptr<Album> album = session_->CreateAlbum();

    sp_album *sp_album = sp_artistbrowse_album(artist_browse_, index);

    album->Load(sp_album);

    return album;
}

int ArtistBrowse::GetNumSimilarArtists() {
    return sp_artistbrowse_num_similar_artists(artist_browse_);
}

boost::shared_ptr<Artist> ArtistBrowse::GetSimilarArtist(int index) {
    boost::shared_ptr<Artist> artist = session_->CreateArtist();

    sp_artist *sp_rtist = sp_artistbrowse_similar_artist(artist_browse_, index);
    artist->Load(sp_rtist);

    return artist;
}

std::string ArtistBrowse::GetBiography() {
    std::string biography = sp_artistbrowse_biography(artist_browse_);

    return biography;
}

void SP_CALLCONV ArtistBrowse::callback_artistbrowse_complete(sp_artistbrowse *result, void *userdata) {
    ArtistBrowse *artist_browse = reinterpret_cast<ArtistBrowse *>(userdata);

    BOOST_ASSERT(artist_browse->artist_browse_ == result);

    artist_browse->OnComplete();
}
}
