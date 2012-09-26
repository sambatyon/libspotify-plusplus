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

namespace spotify {
ArtistBrowse::ArtistBrowse(boost::shared_ptr<Session> session, boost::shared_ptr<Artist> artist) {
    session_ = session;
    artist_ = artist;

    pArtistBrowse_ = sp_artistbrowse_create(session->pSession_, artist->pArtist_, SP_ARTISTBROWSE_FULL, callback_artistbrowse_complete, this);
}

ArtistBrowse::~ArtistBrowse() {
    sp_artistbrowse_release(pArtistBrowse_);
}

bool ArtistBrowse::IsLoading() {
    return !sp_artistbrowse_is_loaded(pArtistBrowse_);
}

boost::shared_ptr<Artist> ArtistBrowse::GetArtist() {
    sp_artist *pArtist = sp_artistbrowse_artist(pArtistBrowse_);

    boost::shared_ptr<Artist> artist = session_->CreateArtist();
    artist->Load(pArtist);

    return artist;
}

int ArtistBrowse::GetNumPortraits() {
    return sp_artistbrowse_num_portraits(pArtistBrowse_);
}

boost::shared_ptr<Image> ArtistBrowse::GetPortrait(int index) {
    boost::shared_ptr<Image> image = session_->CreateImage();

    image->Load(sp_artistbrowse_portrait(pArtistBrowse_, index));

    return image;
}

int ArtistBrowse::GetNumTracks() {
    return sp_artistbrowse_num_tracks(pArtistBrowse_);
}

boost::shared_ptr<Track> ArtistBrowse::GetTrack(int index) {
    boost::shared_ptr<Track> track = session_->CreateTrack();

    sp_track *pTrack = sp_artistbrowse_track(pArtistBrowse_, index);

    track->Load(pTrack);

    return track;
}

int ArtistBrowse::GetNumAlbums() {
    return sp_artistbrowse_num_albums(pArtistBrowse_);
}

boost::shared_ptr<Album> ArtistBrowse::GetAlbum(int index) {
    boost::shared_ptr<Album> album = session_->CreateAlbum();

    sp_album *pAlbum = sp_artistbrowse_album(pArtistBrowse_, index);

    album->Load(pAlbum);

    return album;
}

int ArtistBrowse::GetNumSimilarArtists() {
    return sp_artistbrowse_num_similar_artists(pArtistBrowse_);
}

boost::shared_ptr<Artist> ArtistBrowse::GetSimilarArtist(int index) {
    boost::shared_ptr<Artist> artist = session_->CreateArtist();

    sp_artist *pArtist = sp_artistbrowse_similar_artist(pArtistBrowse_, index);
    artist->Load(pArtist);

    return artist;
}

std::string ArtistBrowse::GetBiography() {
    std::string biography = sp_artistbrowse_biography(pArtistBrowse_);

    return biography;
}

void SP_CALLCONV ArtistBrowse::callback_artistbrowse_complete(sp_artistbrowse *result, void *userdata) {
    ArtistBrowse *pArtistBrowse = reinterpret_cast<ArtistBrowse *>(userdata);

    BOOST_ASSERT(pArtistBrowse->pArtistBrowse_ == result);

    pArtistBrowse->OnComplete();
}
}