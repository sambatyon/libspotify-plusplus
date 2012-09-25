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

// local includes
#include "spotify/Artist.hpp"

namespace Spotify
{

	Artist::Artist( boost::shared_ptr<Session> session )
	{
		m_pArtist = NULL;
		m_session = session;
	}
	
	Artist::~Artist()
	{
		sp_artist_release( m_pArtist );
	}

	void Artist::Load( sp_artist* pArtist )
	{
		m_pArtist = pArtist;
		sp_artist_add_ref( m_pArtist );
	}

	bool Artist::IsLoading()
	{
		return !sp_artist_is_loaded( m_pArtist );
	}

	std::string Artist::GetName()
	{
		return sp_artist_name( m_pArtist );
	}	

	boost::shared_ptr<ArtistBrowse> Artist::Browse()
	{
		return boost::shared_ptr<ArtistBrowse>( new ArtistBrowse( m_session, boost::dynamic_pointer_cast<Artist>( shared_from_this() ) ) );
	}

}
