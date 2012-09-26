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

// Includes
#include "spotify/Session.hpp"

#include "spotify/Album.hpp"
#include "spotify/Artist.hpp"
#include "spotify/Image.hpp"
#include "spotify/PlayList.hpp"
#include "spotify/PlayListContainer.hpp"
#include "spotify/PlayListElement.hpp"
#include "spotify/PlayListFolder.hpp"
#include "spotify/Track.hpp"

// debugging
#include "Debug/Debug.hpp"
#define LOG( msg, ... )	//Debug::PrintLine( msg, __VA_ARGS__ );

namespace spotify {
Session::Config::Config() {
    appKey_ = NULL;
    appKeySize_ = 0;
    cacheLocation_ = "";
    settingsLocation_ = "";
    userAgent_ = "";
    compressPlaylists_ = true;
    dontSaveMetadataForPlaylists_ = false;
    initiallyUnloadPlaylists_ = false;
}

boost::shared_ptr< Session > Session::Create() {
    return boost::shared_ptr< Session >(new Session());
}

Session::Session() : pSession_(NULL), isProcessEventsRequired_(false), hasLoggedOut_(NULL) {
}

Session::~Session() {
    Shutdown();
}

sp_error Session::Initialise(Config &config) {
    sp_session_config spConfig;

    spConfig.api_version = SPOTIFY_API_VERSION;

    // app specified configuration
    spConfig.application_key = config.appKey_;
    spConfig.application_key_size = config.appKeySize_;
    spConfig.cache_location = config.cacheLocation_;
    spConfig.settings_location = config.settingsLocation_;
    spConfig.user_agent = config.userAgent_;

    // locally specified configuration
    sp_session_callbacks callbacks;
    memset(&callbacks, 0, sizeof(callbacks));

    callbacks.connection_error = callback_connection_error;
    callbacks.end_of_track = callback_end_of_track;
    callbacks.get_audio_buffer_stats = callback_get_audio_buffer_stats;
    callbacks.logged_in = callback_logged_in;
    callbacks.logged_out = callback_logged_out;
    callbacks.log_message = callback_log_message;
    callbacks.message_to_user = callback_message_to_user;
    callbacks.metadata_updated = callback_metadata_updated;
    callbacks.music_delivery = callback_music_delivery;
    callbacks.notify_main_thread = callback_notify_main_thread;
    callbacks.play_token_lost = callback_play_token_lost;
    callbacks.start_playback = callback_start_playback;
    callbacks.stop_playback = callback_stop_playback;
    callbacks.streaming_error = callback_streaming_error;
    callbacks.userinfo_updated = callback_userinfo_updated;

    spConfig.callbacks = &callbacks;
    spConfig.userdata = this;

    spConfig.compress_playlists = config.compressPlaylists_;
    spConfig.dont_save_metadata_for_playlists = config.dontSaveMetadataForPlaylists_;
    spConfig.initially_unload_playlists = config.initiallyUnloadPlaylists_;

    sp_error error = sp_session_create(&spConfig, &pSession_);

    return error;
}

void Session::Shutdown() {
    if (pSession_) {
        if (track_) {
            Unload(track_);
        }

        // clear any remaining events
        Update();

        // release the session
        sp_session_release(pSession_);

        pSession_ = NULL;
    }
}

void Session::Update() {
    if (pSession_) {
        isProcessEventsRequired_ = false;

        int nextTimeout = 0;
        sp_session_process_events(pSession_, &nextTimeout);
    }
}

void Session::Login(const char *username, const char *password, bool rememberMe) {
    hasLoggedOut_ = false;

    sp_session_login(pSession_, username, password, rememberMe, NULL);
}

void Session::Logout() {
    sp_session_logout(pSession_);
}

bool Session::IsLoggedIn() {
    bool isloggedIn = (NULL != pSession_) && !hasLoggedOut_ && (GetConnectionState() == SP_CONNECTION_STATE_LOGGED_IN);
    return isloggedIn;
}

sp_connectionstate Session::GetConnectionState() {
    if (pSession_) {
        return sp_session_connectionstate(pSession_);
    } else {
        return SP_CONNECTION_STATE_LOGGED_OUT;
    }
}

sp_error Session::Load(boost::shared_ptr<Track> track) {
    if (track != track_) {
        if (track_) {
            Unload(track_);
            track_.reset();
        }

        if (track) {
            sp_error error = sp_session_player_load(pSession_, track->pTrack_);

            if (error == SP_ERROR_OK) {
                track_ = track;
            }

            return error;
        }
    }

    return SP_ERROR_OK;
}

void Session::Unload(boost::shared_ptr<Track> track) {
    if (track && (track == track_)) {
        sp_session_player_unload(pSession_);
        track_.reset();
    }
}

boost::shared_ptr<Track> Session::GetCurrentTrack() {
    return track_;
}

void Session::Seek(int offset) {
    sp_session_player_seek(pSession_, offset);
}

void Session::Play() {
    sp_session_player_play(pSession_, true);
}

void Session::Stop() {
    sp_session_player_play(pSession_, false);
}

sp_error Session::PreFetch(boost::shared_ptr<Track> track) {
    sp_error error = sp_session_player_prefetch(pSession_, track->pTrack_);
    return error;
}

boost::shared_ptr<PlayListContainer> Session::GetPlayListContainer() {
    sp_playlistcontainer *c = sp_session_playlistcontainer(pSession_);

    if (NULL == c) {
        return boost::shared_ptr<PlayListContainer>();
    }

    boost::shared_ptr<PlayListContainer> container = CreatePlayListContainer();
    bool isLoading = container->Load(c);

    if (!isLoading) {
        container.reset();
    }

    return container;
}

boost::shared_ptr<PlayList> Session::GetStarredPlayList() {
    sp_playlist *p = sp_session_starred_create(pSession_);

    if (NULL == p) {
        return boost::shared_ptr<PlayList>();
    }

    boost::shared_ptr<PlayList> playList = CreatePlayList();
    bool isLoading = playList->Load(p);

    if (!isLoading) {
        playList.reset();
    }

    return playList;
}

void Session::SetPreferredBitrate(sp_bitrate bitrate) {
    sp_session_preferred_bitrate(pSession_, bitrate);
}

boost::shared_ptr<PlayList> Session::CreatePlayList() {
    return boost::shared_ptr<PlayList>(new PlayList(shared_from_this()));
}

boost::shared_ptr<PlayListContainer> Session::CreatePlayListContainer() {
    return boost::shared_ptr<PlayListContainer>(new PlayListContainer(shared_from_this()));
}

boost::shared_ptr<PlayListFolder> Session::CreatePlayListFolder() {
    return boost::shared_ptr<PlayListFolder>(new PlayListFolder(shared_from_this()));
}

boost::shared_ptr<Track> Session::CreateTrack() {
    return boost::shared_ptr<Track>(new Track(shared_from_this()));
}

boost::shared_ptr<Artist> Session::CreateArtist() {
    return boost::shared_ptr<Artist>(new Artist(shared_from_this()));
}

boost::shared_ptr<Album> Session::CreateAlbum() {
    return boost::shared_ptr<Album>(new Album(shared_from_this()));
}

boost::shared_ptr<Image> Session::CreateImage() {
    return boost::shared_ptr<Image>(new Image(shared_from_this()));
}

namespace {
static Session *GetSessionFromUserdata(sp_session *session) {
    Session *pSession = reinterpret_cast<Session *>(sp_session_userdata(session));
    return pSession;
}
}

void SP_CALLCONV Session::callback_logged_in(sp_session *session, sp_error error) {
    Session *pSession = GetSessionFromUserdata(session);
    pSession->OnLoggedIn(error);
}

void SP_CALLCONV Session::callback_logged_out(sp_session *session) {
    Session *pSession = GetSessionFromUserdata(session);

    pSession->hasLoggedOut_ = true;

    pSession->OnLoggedOut();
}

void SP_CALLCONV Session::callback_metadata_updated(sp_session *session) {
    Session *pSession = GetSessionFromUserdata(session);
    pSession->OnMetadataUpdated();
}

void SP_CALLCONV Session::callback_connection_error(sp_session *session, sp_error error) {
    Session *pSession = GetSessionFromUserdata(session);
    pSession->OnConnectionError(error);
}

void SP_CALLCONV Session::callback_message_to_user(sp_session *session, const char *message) {
    Session *pSession = GetSessionFromUserdata(session);
    pSession->OnMessageToUser(message);
}

void SP_CALLCONV Session::callback_notify_main_thread(sp_session *session) {
    Session *pSession = GetSessionFromUserdata(session);

    pSession->OnNotifyMainThread();

    pSession->isProcessEventsRequired_ = true;
}

int  SP_CALLCONV Session::callback_music_delivery(sp_session *session, const sp_audioformat *format, const void *frames, int num_frames) {
    Session *pSession = GetSessionFromUserdata(session);
    return pSession->OnMusicDelivery(format, frames, num_frames);
}

void SP_CALLCONV Session::callback_play_token_lost(sp_session *session) {
    Session *pSession = GetSessionFromUserdata(session);
    pSession->OnPlayTokenLost();
}

void SP_CALLCONV Session::callback_log_message(sp_session *session, const char *data) {
    Session *pSession = GetSessionFromUserdata(session);
    pSession->OnLogMessage(data);
}

void SP_CALLCONV Session::callback_end_of_track(sp_session *session) {
    Session *pSession = GetSessionFromUserdata(session);
    pSession->OnEndOfTrack();
}

void SP_CALLCONV Session::callback_streaming_error(sp_session *session, sp_error error) {
    Session *pSession = GetSessionFromUserdata(session);
    pSession->OnStreamingError(error);
}

void SP_CALLCONV Session::callback_userinfo_updated(sp_session *session) {
    Session *pSession = GetSessionFromUserdata(session);
    pSession->OnUserinfoUpdated();
}

void SP_CALLCONV Session::callback_start_playback(sp_session *session) {
    Session *pSession = GetSessionFromUserdata(session);
    pSession->OnStartPlayback();
}

void SP_CALLCONV Session::callback_stop_playback(sp_session *session) {
    Session *pSession = GetSessionFromUserdata(session);
    pSession->OnStopPlayback();
}

void SP_CALLCONV Session::callback_get_audio_buffer_stats(sp_session *session, sp_audio_buffer_stats *stats) {
    Session *pSession = GetSessionFromUserdata(session);
    pSession->OnGetAudioBufferStats(stats);
}

void Session::OnLoggedIn(sp_error error) {
    LOG("Session::OnLoggedIn");
}

void Session::OnLoggedOut() {
    LOG("Session::OnLoggedOut");
}

void Session::OnMetadataUpdated() {
    LOG("Session::OnMetadataUpdated");
}

void Session::OnConnectionError(sp_error error) {
    LOG("Session::OnConnectionError");
}

void Session::OnMessageToUser(const char *message) {
    LOG("Session::OnMessageToUser");
    LOG(message);
}

void Session::OnNotifyMainThread() {
    LOG("Session::OnNotifyMainThread");
}

int  Session::OnMusicDelivery(const sp_audioformat *format, const void *frames, int num_frames) {
    LOG("Session::OnMusicDelivery [%d]", ((int *)frames)[0]);

    // pretend that we have consumed all of the audio frames
    return num_frames;
}

void Session::OnPlayTokenLost() {
    LOG("Session::OnPlayTokenLost");
}

void Session::OnLogMessage(const char *data) {
    LOG("Session::OnLogMessage");
    LOG(data);
}

void Session::OnEndOfTrack() {
    LOG("Session::OnEndOfTrack");
}

void Session::OnStreamingError(sp_error error) {
    LOG("Session::OnStreamingError");
}

void Session::OnUserinfoUpdated() {
    LOG("Session::OnUserinfoUpdated");
}

void Session::OnStartPlayback() {
    LOG("Session::OnStartPlayback");
}

void Session::OnStopPlayback() {
    LOG("Session::OnStopPlayback");
}

void Session::OnGetAudioBufferStats(sp_audio_buffer_stats *stats) {
    LOG("Session::OnGetAudioBufferStats");
}

}
