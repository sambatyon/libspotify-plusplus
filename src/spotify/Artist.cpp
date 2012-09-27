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
#include "spotify/Artist.hpp"

namespace spotify {
Artist::Artist(boost::shared_ptr<Session> session) : artist_(NULL), session_(session) {
}

Artist::~Artist() {
    sp_artist_release(artist_);
}

void Artist::Load(sp_artist *pArtist) {
    artist_ = pArtist;
    sp_artist_add_ref(artist_);
}

bool Artist::IsLoading() {
    return !sp_artist_is_loaded(artist_);
}

std::string Artist::GetName() {
    return sp_artist_name(artist_);
}

boost::shared_ptr<ArtistBrowse> Artist::Browse() {
    return boost::shared_ptr<ArtistBrowse>(new ArtistBrowse(session_, boost::dynamic_pointer_cast<Artist>(shared_from_this())));
}
}
