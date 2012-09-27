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

// Local Includes
#include "spotify/LibConfig.hpp"
#include "spotify/PlayListElement.hpp"

// boost includes
#include <boost/shared_ptr.hpp>

// std includes
#include <vector>
#include <string>

// libspotify Includes
#include <libspotify/api.h>

namespace spotify {
// forward declaration
class Session;

class LIBSPOTIFYPP_API PlayListFolder : public PlayListElement {
  public:
    PlayListFolder(boost::shared_ptr<Session> session);
    virtual ~PlayListFolder();

    virtual bool Load(sp_playlistcontainer *container, int index);
    virtual void Unload();

    virtual void AddPlayList(boost::shared_ptr<PlayListElement> playList);

    virtual bool IsLoading(bool recursive);

    virtual PlayListType GetType();

    virtual void DumpToTTY(int level = 0);

    virtual std::string GetName();

    virtual sp_uint64 GetGroupID();

    virtual bool HasChildren();
    virtual int GetNumChildren();
    virtual boost::shared_ptr<PlayListElement> GetChild(int index);

  private:
    typedef std::vector<boost::shared_ptr<PlayListElement>> PlayListStore;
    PlayListStore playlists_;

    sp_playlistcontainer *container_;
    int container_index_;
};
}
