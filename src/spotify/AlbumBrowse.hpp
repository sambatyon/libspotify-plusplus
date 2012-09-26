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

#pragma once

// std include
#include <string>

// libspotify include
#include "libspotify/api.h"

// boost includes
#include <boost/shared_ptr.hpp>

namespace spotify {
// forward declaration
class Session;
class Album;
class Artist;
class Track;
class Disc;

class AlbumBrowse {
public:
    AlbumBrowse(boost::shared_ptr<Session> session, boost::shared_ptr<Album> album);
    ~AlbumBrowse();

    bool IsLoading();

    boost::shared_ptr<Album> GetAlbum();

    boost::shared_ptr<Artist> GetArtist();

    int GetNumCopyrights();

    std::string GetCopyright(int index);

    std::string GetReview();

    int GetNumTracks();

    boost::shared_ptr<Track> GetTrack(int index);

    int GetNumDiscs();

    boost::shared_ptr<Disc> GetDisc(int index);

protected:

    virtual void OnComplete() {}

private:

    static void SP_CALLCONV callback_albumbrowse_complete(sp_albumbrowse *result, void *userdata);

    boost::shared_ptr<Session> session_;
    boost::shared_ptr<Album> album_;

    sp_albumbrowse	*pAlbumBrowse_;
};

}