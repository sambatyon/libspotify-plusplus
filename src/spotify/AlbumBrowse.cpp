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
#include "spotify/AlbumBrowse.hpp"
#include "spotify/Session.hpp"
#include "spotify/Album.hpp"
#include "spotify/Disc.hpp"

#include <log4cplus/logger.h>

namespace spotify {
namespace {
log4cplus::Logger logger = log4cplus::Logger::getInstance("spotify.AlbumBrowse");
}
AlbumBrowse::AlbumBrowse(boost::shared_ptr<Session> session, boost::shared_ptr<Album> album) : session_(session)
                                                                                             , album_(album)
                                                                                             , album_browse_(NULL) {
    album_browse_ = sp_albumbrowse_create(session->session_, album->album_, callback_albumbrowse_complete, this);
}

AlbumBrowse::~AlbumBrowse() {
    sp_albumbrowse_release(album_browse_);
}

bool AlbumBrowse::IsLoading() {
    return !sp_albumbrowse_is_loaded(album_browse_);
}

boost::shared_ptr<Album> AlbumBrowse::GetAlbum() {
    return album_;
}

boost::shared_ptr<Artist> AlbumBrowse::GetArtist() {
    return album_->GetArtist();
}

int AlbumBrowse::GetNumCopyrights() {
    return sp_albumbrowse_num_copyrights(album_browse_);
}

std::string AlbumBrowse::GetCopyright(int index) {
    std::string copyright = sp_albumbrowse_copyright(album_browse_, index);

    return copyright;
}

std::string AlbumBrowse::GetReview() {
    std::string review = sp_albumbrowse_review(album_browse_);

    return review;
}

int AlbumBrowse::GetNumTracks() {
    return sp_albumbrowse_num_tracks(album_browse_);
}

int AlbumBrowse::GetNumDiscs() {
    /// @todo parse the tracks for this album, to determine their disc indices, and thus the number of discs available
    return 0;
}

boost::shared_ptr<Disc> AlbumBrowse::GetDisc(int index) {
    /// @todo implement this function
    return boost::shared_ptr<Disc>();
}

void SP_CALLCONV AlbumBrowse::callback_albumbrowse_complete(sp_albumbrowse *result, void *userdata) {
    AlbumBrowse *album_browse = reinterpret_cast<AlbumBrowse *>(userdata);

    BOOST_ASSERT(album_browse->album_browse_ == result);

    album_browse->OnComplete();
}
}
