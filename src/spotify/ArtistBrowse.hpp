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

#pragma once

// local includes
#include "spotify/Image.hpp"
#include "spotify/Track.hpp"
#include "spotify/Album.hpp"

// std include
#include <string>

// libspotify include
#include "libspotify/api.h"

// boost includes
#include <boost/shared_ptr.hpp>

namespace spotify {
// forward declaration
class Session;
class Artist;

class ArtistBrowse {
public:
    ArtistBrowse(boost::shared_ptr<Session> session, boost::shared_ptr<Artist> artist);
    ~ArtistBrowse();

    bool IsLoading();

    boost::shared_ptr<Artist> GetArtist();

    int GetNumPortraits();

    boost::shared_ptr<Image> GetPortrait(int index);

    int GetNumTracks();

    boost::shared_ptr<Track> GetTrack(int index);

    int GetNumAlbums();

    boost::shared_ptr<Album> GetAlbum(int index);

    int GetNumSimilarArtists();

    boost::shared_ptr<Artist> GetSimilarArtist(int index);

    std::string GetBiography();

protected:

    virtual void OnComplete() {}

private:

    static void SP_CALLCONV callback_artistbrowse_complete(sp_artistbrowse *result, void *userdata);

    boost::shared_ptr<Session> session_;
    boost::shared_ptr<Artist> artist_;

    sp_artistbrowse *pArtistBrowse_;
};


}
