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

// Includes
#include "spotify/Session.hpp"

#include <log4cplus/loggingmacros.h>
#include <log4cplus/logger.h>

#include <boost/make_shared.hpp>
#include <boost/format.hpp>

#include "spotify/Album.hpp"
#include "spotify/Artist.hpp"
#include "spotify/Image.hpp"
#include "spotify/PlayList.hpp"
#include "spotify/PlayListContainer.hpp"
#include "spotify/PlayListElement.hpp"
#include "spotify/PlayListFolder.hpp"
#include "spotify/Track.hpp"

namespace spotify {
namespace {
log4cplus::Logger logger = log4cplus::Logger::getInstance("spotify.Session");
}

Config::Config() {
    app_key = NULL;
    app_key_size = 0;
    cache_location = "";
    settings_ocation = "";
    user_agent = "";
    compress_playlists = true;
    dont_saveMetadata_for_playlists = false;
    initially_unload_playlists = false;
}

boost::shared_ptr<Session> Session::Create() {
    return boost::shared_ptr<Session>(boost::make_shared<Session>());
}

Session::Session() : session_(), is_process_events_required_(false), has_logged_out_(NULL) {
}

Session::~Session() {
    Shutdown();
}

sp_error Session::Initialise(const Config &config) {
    sp_session_config sp_config = {0};

    sp_config.api_version = SPOTIFY_API_VERSION;

    // app specified configuration
    sp_config.application_key = config.app_key;
    sp_config.application_key_size = config.app_key_size;
    sp_config.cache_location = config.cache_location;
    sp_config.settings_location = config.settings_ocation;
    sp_config.user_agent = config.user_agent;

    // locally specified configuration
    sp_session_callbacks callbacks = {0};

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

    sp_config.callbacks = &callbacks;
    sp_config.userdata = this;

    sp_config.compress_playlists = config.compress_playlists;
    sp_config.dont_save_metadata_for_playlists = config.dont_saveMetadata_for_playlists;
    sp_config.initially_unload_playlists = config.initially_unload_playlists;

    sp_error error = sp_session_create(&sp_config, &session_);

    return error;
}

void Session::Shutdown() {
    if (session_) {
        if (track_)
            Unload(track_);
        // clear any remaining events
        Update();
        // release the session
        // for some reason, the session release generates segfaults
        // sp_session_release(session_);
        session_ = NULL;
    }
}

int Session::Update() {
    if (session_) {
        is_process_events_required_ = false;
        int next_timeout = 0;
        sp_session_process_events(session_, &next_timeout);
        return next_timeout;
    }
    return -1;
}

void Session::Login(const char *username, const char *password, bool remember_me) {
    has_logged_out_ = false;
    sp_session_login(session_, username, password, remember_me, NULL);
}

void Session::Logout() {
    sp_session_logout(session_);
}

bool Session::IsLoggedIn() {
    sp_connectionstate state = GetConnectionState();
    LOG4CPLUS_TRACE(logger, (boost::format("conn state: %d") % state));
    bool is_logged_in = session_ && !has_logged_out_ && (state == SP_CONNECTION_STATE_LOGGED_IN);
    return is_logged_in;
}

sp_connectionstate Session::GetConnectionState() {
    if (session_)
        return sp_session_connectionstate(session_);
    else
        return SP_CONNECTION_STATE_LOGGED_OUT;
}

sp_error Session::Load(boost::shared_ptr<Track> track) {
    if (track != track_) {
        if (track_) {
            Unload(track_);
            track_.reset();
        }

        if (track) {
            sp_error error = sp_session_player_load(session_, track->track_);
            if (error == SP_ERROR_OK)
                track_ = track;
            return error;
        }
    }

    return SP_ERROR_OK;
}

void Session::Unload(boost::shared_ptr<Track> track) {
    if (track && (track == track_)) {
        sp_session_player_unload(session_);
        track_.reset();
    }
}

boost::shared_ptr<Track> Session::GetCurrentTrack() {
    return track_;
}

void Session::Seek(int offset) {
    sp_session_player_seek(session_, offset);
}

void Session::Play() {
    sp_session_player_play(session_, true);
}

void Session::Stop() {
    sp_session_player_play(session_, false);
}

sp_error Session::PreFetch(boost::shared_ptr<Track> track) {
    sp_error error = sp_session_player_prefetch(session_, track->track_);
    return error;
}

boost::shared_ptr<PlayListContainer> Session::GetPlayListContainer() {
    sp_playlistcontainer *c = sp_session_playlistcontainer(session_);

    if (!c)
        return boost::shared_ptr<PlayListContainer>();

    boost::shared_ptr<PlayListContainer> container = CreatePlayListContainer();
    bool loading = container->Load(c);

    if (!loading)
        container.reset();

    return container;
}

boost::shared_ptr<PlayList> Session::GetStarredPlayList() {
    sp_playlist *p = sp_session_starred_create(session_);

    if (!p)
        return boost::shared_ptr<PlayList>();

    boost::shared_ptr<PlayList> playList = CreatePlayList();
    bool loading = playList->Load(p);

    if (!loading)
        playList.reset();

    return playList;
}

void Session::SetPreferredBitrate(sp_bitrate bitrate) {
    sp_session_preferred_bitrate(session_, bitrate);
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

void Session::connectToOnLoggedIn(boost::function<void (sp_error)> callback) { // NOLINT
    on_loggedin_.connect(callback);
}

void Session::connectToOnNotifyMainThread(boost::function<void ()> callback) { // NOLINT
    on_notify_main_thread_.connect(callback);
}

namespace {
inline
Session *GetSessionFromUserdata(sp_session *session) {
    Session *sess = reinterpret_cast<Session *>(sp_session_userdata(session));
    return sess;
}
}

void SP_CALLCONV Session::callback_logged_in(sp_session *session, sp_error error) {
    Session *sess = GetSessionFromUserdata(session);
    sess->OnLoggedIn(error);
}

void SP_CALLCONV Session::callback_logged_out(sp_session *session) {
    Session *sess = GetSessionFromUserdata(session);
    sess->has_logged_out_ = true;
    sess->OnLoggedOut();
}

void SP_CALLCONV Session::callback_metadata_updated(sp_session *session) {
    Session *sess = GetSessionFromUserdata(session);
    sess->OnMetadataUpdated();
}

void SP_CALLCONV Session::callback_connection_error(sp_session *session, sp_error error) {
    Session *sess = GetSessionFromUserdata(session);
    sess->OnConnectionError(error);
}

void SP_CALLCONV Session::callback_message_to_user(sp_session *session, const char *message) {
    Session *sess = GetSessionFromUserdata(session);
    sess->OnMessageToUser(message);
}

void SP_CALLCONV Session::callback_notify_main_thread(sp_session *session) {
    Session *sess = GetSessionFromUserdata(session);
    sess->OnNotifyMainThread();
    sess->is_process_events_required_ = true;
}

int  SP_CALLCONV Session::callback_music_delivery(sp_session *session, const sp_audioformat *format,
                                                  const void *frames, int num_frames) {
    Session *sess = GetSessionFromUserdata(session);
    return sess->OnMusicDelivery(format, frames, num_frames);
}

void SP_CALLCONV Session::callback_play_token_lost(sp_session *session) {
    Session *sess = GetSessionFromUserdata(session);
    sess->OnPlayTokenLost();
}

void SP_CALLCONV Session::callback_log_message(sp_session *session, const char *data) {
    Session *sess = GetSessionFromUserdata(session);
    sess->OnLogMessage(data);
}

void SP_CALLCONV Session::callback_end_of_track(sp_session *session) {
    Session *sess = GetSessionFromUserdata(session);
    sess->OnEndOfTrack();
}

void SP_CALLCONV Session::callback_streaming_error(sp_session *session, sp_error error) {
    Session *sess = GetSessionFromUserdata(session);
    sess->OnStreamingError(error);
}

void SP_CALLCONV Session::callback_userinfo_updated(sp_session *session) {
    Session *sess = GetSessionFromUserdata(session);
    sess->OnUserinfoUpdated();
}

void SP_CALLCONV Session::callback_start_playback(sp_session *session) {
    Session *sess = GetSessionFromUserdata(session);
    sess->OnStartPlayback();
}

void SP_CALLCONV Session::callback_stop_playback(sp_session *session) {
    Session *sess = GetSessionFromUserdata(session);
    sess->OnStopPlayback();
}

void SP_CALLCONV Session::callback_get_audio_buffer_stats(sp_session *session, sp_audio_buffer_stats *stats) {
    Session *sess = GetSessionFromUserdata(session);
    sess->OnGetAudioBufferStats(stats);
}

void Session::OnLoggedIn(sp_error error) {
    LOG4CPLUS_TRACE(logger, (boost::format("Session::OnLoggedIn: %s") % sp_error_message(error)));
    on_loggedin_(error);
}

void Session::OnLoggedOut() {
    LOG4CPLUS_TRACE(logger, "Session::OnLoggedOut");
}

void Session::OnMetadataUpdated() {
    LOG4CPLUS_TRACE(logger, "Session::OnMetadataUpdated");
}

void Session::OnConnectionError(sp_error error) {
    LOG4CPLUS_TRACE(logger, "Session::OnConnectionError");
}

void Session::OnMessageToUser(const char *message) {
    LOG4CPLUS_TRACE(logger, "Session::OnMessageToUser");
    LOG4CPLUS_INFO(logger, message);
}

void Session::OnNotifyMainThread() {
    LOG4CPLUS_TRACE(logger, "Session::OnNotifyMainThread");
    on_notify_main_thread_();
}

int  Session::OnMusicDelivery(const sp_audioformat *format, const void *frames, int num_frames) {
    LOG4CPLUS_TRACE(logger, (boost::format("Session::OnMusicDelivery [%d]")
                                           % (reinterpret_cast<const int*>(frames))[0]));

    // pretend that we have consumed all of the audio frames
    return num_frames;
}

void Session::OnPlayTokenLost() {
    LOG4CPLUS_TRACE(logger, "Session::OnPlayTokenLost");
}

void Session::OnLogMessage(const char *data) {
    LOG4CPLUS_TRACE(logger, "Session::OnLogMessage");
    LOG4CPLUS_TRACE(logger, data);
}

void Session::OnEndOfTrack() {
    LOG4CPLUS_TRACE(logger, "Session::OnEndOfTrack");
}

void Session::OnStreamingError(sp_error error) {
    LOG4CPLUS_ERROR(logger, "Session::OnStreamingError");
}

void Session::OnUserinfoUpdated() {
    LOG4CPLUS_TRACE(logger, "Session::OnUserinfoUpdated");
}

void Session::OnStartPlayback() {
    LOG4CPLUS_TRACE(logger, "Session::OnStartPlayback");
}

void Session::OnStopPlayback() {
    LOG4CPLUS_TRACE(logger, "Session::OnStopPlayback");
}

void Session::OnGetAudioBufferStats(sp_audio_buffer_stats *stats) {
    LOG4CPLUS_TRACE(logger, "Session::OnGetAudioBufferStats");
}
}
