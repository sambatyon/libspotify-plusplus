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

// lib includes
#include "Spotify/Track.h"
#include "Spotify/Session.h"

// Local includes
#include "Debug/Debug.h"

namespace Spotify
{

	Track::Track( boost::shared_ptr<Session> session ) : PlayListElement( session )
	{
	}

	Track::~Track()
	{		
		Unload();
	}
	
	bool Track::Load( sp_track* track )
	{
		m_pTrack = track;
		sp_track_add_ref( m_pTrack );
		return true;
	}

	void Track::Unload()
	{
		sp_track_release( m_pTrack );
		m_pTrack = NULL;
	}

	bool Track::IsLoading( bool recursive )
	{
		if (!sp_track_is_loaded(m_pTrack))
			return true;

		return false;
	}			

	std::string Track::GetName()
	{
		const char* name = sp_track_name( m_pTrack );
		return name;
	}

	int Track::GetDuration()
	{
		int duration = sp_track_duration( m_pTrack );			
		return duration;
	}

	PlayListElement::eType Track::GetType()
	{
		return TRACK;
	}

	bool Track::HasChildren()
	{
		return false;
	}

	int Track::GetNumChildren()
	{
		return 0;
	}

	boost::shared_ptr<PlayListElement> Track::GetChild( int index )
	{
		return boost::shared_ptr<PlayListElement>();
	}

	void Track::DumpToTTY( int level )
	{
		int seconds = GetDuration() / 1000;
		int mins = seconds / 60;
		seconds %= 60;
		Debug::PrintLine( level, "Track [%s] [%d]mins [%d]secs", GetName().c_str(), mins, seconds);
	}

	int Track::GetNumArtists()
	{
		return sp_track_num_artists( m_pTrack );
	}

	boost::shared_ptr<Artist> Track::GetArtist( int index )
	{
		boost::shared_ptr<Artist> artist = m_session->CreateArtist();
		artist->Load( sp_track_artist( m_pTrack, index ) );

		return artist;
	}

	boost::shared_ptr<Album> Track::GetAlbum()
	{
		boost::shared_ptr<Album> album = m_session->CreateAlbum();
		album->Load( sp_track_album( m_pTrack ) );

		return album;
	}

	int Track::GetDisc()
	{
		return sp_track_disc( m_pTrack );
	}

	int Track::GetPopularity()
	{
		return sp_track_popularity( m_pTrack );
	}

	bool Track::IsStarred()
	{
		return sp_track_is_starred( m_session->m_pSession, m_pTrack );
	}

	void Track::SetStarred( bool isStarred )
	{		
		sp_track_set_starred( m_session->m_pSession, &m_pTrack, 1, isStarred );
	}
}
