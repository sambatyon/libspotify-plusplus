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

// libspotify include
#include <libspotify/api.h>

// std includes
#include <vector>
#include <string>

// boost includes
#include <boost/shared_ptr.hpp>

// local includes
#include "spotify/LibConfig.hpp"
#include "spotify/Playlist.hpp"

namespace spotify {
// forward declaration
class Session;

class LIBSPOTIFYPP_API PlayListContainer : public PlayListElement {
  public:
    explicit PlayListContainer(boost::shared_ptr<Session> session);
    virtual ~PlayListContainer();

    virtual PlayListType GetType();

    virtual bool Load(sp_playlistcontainer *container);
    virtual void Unload();
    virtual bool IsLoading(bool recursive);

    virtual void AddPlayList(boost::shared_ptr<PlayListElement> playlist);

    virtual bool HasChildren();
    virtual int GetNumChildren();
    virtual boost::shared_ptr<PlayListElement> GetChild(int index);

    virtual std::string GetName();

    virtual void DumpToTTY(int level = 0);

  protected:
    virtual void OnPlaylistAdded(sp_playlist *playlist, int position);
    virtual void OnPlaylistRemoved(sp_playlist *playlist, int position);
    virtual void OnPlaylistMoved(sp_playlist *playlist, int position, int new_position);
    virtual void OnContainerLoaded();

  private:
    friend class Session;

    static void SP_CALLCONV callback_playlist_added(sp_playlistcontainer *pc, sp_playlist *playlist, int position,
                                                    void *userdata);
    static void SP_CALLCONV callback_playlist_removed(sp_playlistcontainer *pc, sp_playlist *playlist, int position,
                                                      void *userdata);
    static void SP_CALLCONV callback_playlist_moved(sp_playlistcontainer *pc, sp_playlist *playlist, int position,
                                                    int new_position, void *userdata);
    static void SP_CALLCONV callback_container_loaded(sp_playlistcontainer *pc, void *userdata);

    static void GetCallbacks(sp_playlistcontainer_callbacks *callbacks);
    static PlayListContainer *GetPlayListContainer(sp_playlistcontainer *pc, void *userdata);

    sp_playlistcontainer *container_;
    bool loading_;
    typedef std::vector<boost::shared_ptr<PlayListElement>> PlayListStore;
    PlayListStore playlists_;
};
}
