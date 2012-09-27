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
#include "spotify/Album.hpp"
#include "spotify/Image.hpp"
#include "spotify/Session.hpp"
#include "spotify/Artist.hpp"

namespace spotify {
Album::Album(boost::shared_ptr<Session> session) : album_(NULL), session_(session) {
}

Album::~Album() {
    sp_album_release(album_);
}

void Album::Load(sp_album *pAlbum) {
    album_ = pAlbum;
    sp_album_add_ref(album_);
}

bool Album::IsLoading() {
    return !sp_album_is_loaded(album_);
}

std::string Album::GetName() {
    return sp_album_name(album_);
}

boost::shared_ptr<Image> Album::GetImage() {
    if (IsLoading())
        return boost::shared_ptr<Image>();

    const byte *album_id = sp_album_cover(album_, SP_IMAGE_SIZE_LARGE);

    if (!album_id)
        return boost::shared_ptr<Image>();

    boost::shared_ptr<Image> image = session_->CreateImage();

    if (image->Load(album_id))
        return image;
    else
        return boost::shared_ptr<Image>();
}

boost::shared_ptr<AlbumBrowse> Album::Browse() {
    return boost::shared_ptr<AlbumBrowse>(new AlbumBrowse(session_, 
                                                          boost::dynamic_pointer_cast<Album>(shared_from_this())));
}

boost::shared_ptr<Artist> Album::GetArtist() {
    sp_artist *sp_artist = sp_album_artist(album_);
    boost::shared_ptr<Artist> artist = session_->CreateArtist();
    artist->Load(sp_artist);
    return artist;
}
}
