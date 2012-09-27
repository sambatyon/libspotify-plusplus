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

// libspotify includes
#include <libspotify/api.h>

// C-libs includes
#include <cstdint>

// boost includes
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "spotify/LibConfig.hpp"

namespace spotify {
class Album;
class Artist;
class Image;
class PlayList;
class PlayListContainer;
class PlayListElement;
class PlayListFolder;
class Track;
class AlbumBrowse;
class ArtistBrowse;

namespace core {
// forward declaration
class Mutex;
}

class LIBSPOTIFYPP_API Session : public boost::enable_shared_from_this<Session> {
  public:
    struct Config {
        Config();

        const std::uint8_t *app_key;
        std::size_t app_key_size;
        const char *cache_location;
        const char *settings_ocation;
        const char *user_agent;
        bool compress_playlists;
        bool dont_saveMetadata_for_playlists;
        bool initially_unload_playlists;
    };

    static boost::shared_ptr<Session> Create();

    virtual ~Session();

    virtual sp_error Initialise(const Config &config);

    virtual void Update();

    virtual void Login(const char *username, const char *password, bool rememberMe = false);
    virtual void Logout();

    virtual bool IsLoggedIn();

    virtual sp_connectionstate GetConnectionState();

    virtual sp_error Load(boost::shared_ptr<Track> track);
    virtual void Unload(boost::shared_ptr<Track> track);
    virtual boost::shared_ptr<Track> GetCurrentTrack();
    virtual void Seek(int offset);
    virtual void Play();
    virtual void Stop();

    virtual sp_error PreFetch(boost::shared_ptr<Track> track);

    virtual boost::shared_ptr<PlayListContainer> GetPlayListContainer();

    virtual boost::shared_ptr<PlayList> GetStarredPlayList();

    virtual void SetPreferredBitrate(sp_bitrate bitrate);

    // factory functions
    virtual boost::shared_ptr<PlayList> CreatePlayList();
    virtual boost::shared_ptr<PlayListContainer> CreatePlayListContainer();
    virtual boost::shared_ptr<PlayListFolder> CreatePlayListFolder();
    virtual boost::shared_ptr<Track> CreateTrack();
    virtual boost::shared_ptr<Artist> CreateArtist();
    virtual boost::shared_ptr<Album> CreateAlbum();
    virtual boost::shared_ptr<Image> CreateImage();

  protected:
    virtual void Shutdown();

    // protected constructor, to force use of Create()
    Session();

    // C++ Style member callbacks
    virtual void OnLoggedIn(sp_error error);
    virtual void OnLoggedOut();
    virtual void OnMetadataUpdated();
    virtual void OnConnectionError(sp_error error);
    virtual void OnMessageToUser(const char *message);
    virtual void OnNotifyMainThread();
    virtual int  OnMusicDelivery(const sp_audioformat *format, const void *frames, int num_frames);
    virtual void OnPlayTokenLost();
    virtual void OnLogMessage(const char *data);
    virtual void OnEndOfTrack();
    virtual void OnStreamingError(sp_error error);
    virtual void OnUserinfoUpdated();
    virtual void OnStartPlayback();
    virtual void OnStopPlayback();
    virtual void OnGetAudioBufferStats(sp_audio_buffer_stats *stats);

  private:
    friend class Image;
    friend class Track;
    friend class ArtistBrowse;
    friend class AlbumBrowse;

    // C Style Static callbacks
    static void SP_CALLCONV callback_logged_in(sp_session *session, sp_error error);
    static void SP_CALLCONV callback_logged_out(sp_session *session);
    static void SP_CALLCONV callback_metadata_updated(sp_session *session);
    static void SP_CALLCONV callback_connection_error(sp_session *session, sp_error error);
    static void SP_CALLCONV callback_message_to_user(sp_session *session, const char *message);
    static void SP_CALLCONV callback_notify_main_thread(sp_session *session);
    static int  SP_CALLCONV callback_music_delivery(sp_session *session, const sp_audioformat *format,
                                                    const void *frames, int num_frames);
    static void SP_CALLCONV callback_play_token_lost(sp_session *session);
    static void SP_CALLCONV callback_log_message(sp_session *session, const char *data);
    static void SP_CALLCONV callback_end_of_track(sp_session *session);
    static void SP_CALLCONV callback_streaming_error(sp_session *session, sp_error error);
    static void SP_CALLCONV callback_userinfo_updated(sp_session *session);
    static void SP_CALLCONV callback_start_playback(sp_session *session);
    static void SP_CALLCONV callback_stop_playback(sp_session *session);
    static void SP_CALLCONV callback_get_audio_buffer_stats(sp_session *session, sp_audio_buffer_stats *stats);

    sp_session *session_;
    volatile bool is_process_events_required_;
    volatile bool has_logged_out_;
    boost::shared_ptr<Track> track_;  // currently playing track
};
}
