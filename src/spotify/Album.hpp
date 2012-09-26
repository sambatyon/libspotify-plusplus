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

// local includes
#include "spotify/AlbumBrowse.hpp"

// std include
#include <string>

// libspotify include
#include "libspotify/api.h"

// boost includes
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace spotify {
// forward declarations
class Image;
class Session;
class Artist;

class Album : public boost::enable_shared_from_this<Album> {
public:

    Album(boost::shared_ptr<Session> session);
    virtual ~Album();

    virtual void Load(sp_album *pAlbum);

    virtual bool IsLoading();

    virtual std::string GetName();

    virtual boost::shared_ptr<Image> GetImage();

    virtual boost::shared_ptr<AlbumBrowse> Browse();

    virtual boost::shared_ptr<Artist> GetArtist();

protected:
    friend class AlbumBrowse;

    sp_album		*pAlbum_;
    boost::shared_ptr<Session>	session_;
};

}