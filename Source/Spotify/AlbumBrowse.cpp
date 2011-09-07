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
#include "Spotify/AlbumBrowse.h"
#include "Spotify/Session.h"
#include "Spotify/Album.h"
#include "Spotify/Disc.h"

namespace Spotify
{
	AlbumBrowse::AlbumBrowse( boost::shared_ptr<Session> session, boost::shared_ptr<Album> album )
	{
		m_session = session;
		m_album = album;

		m_pAlbumBrowse = sp_albumbrowse_create( session->m_pSession, album->m_pAlbum, callback_albumbrowse_complete, this );
	}

	AlbumBrowse::~AlbumBrowse()
	{
		sp_albumbrowse_release( m_pAlbumBrowse );
	}

	bool AlbumBrowse::IsLoading()
	{
		return !sp_albumbrowse_is_loaded( m_pAlbumBrowse );
	}

	boost::shared_ptr<Album> AlbumBrowse::GetAlbum()
	{
		return m_album;
	}

	boost::shared_ptr<Artist> AlbumBrowse::GetArtist()
	{
		return m_album->GetArtist();
	}

	int AlbumBrowse::GetNumCopyrights()
	{
		return sp_albumbrowse_num_copyrights( m_pAlbumBrowse );
	}

	std::string AlbumBrowse::GetCopyright( int index )
	{
		std::string copyright = sp_albumbrowse_copyright( m_pAlbumBrowse, index );

		return copyright;
	}
		
	std::string AlbumBrowse::GetReview()
	{
		std::string review = sp_albumbrowse_review( m_pAlbumBrowse );

		return review;
	}
    
    int AlbumBrowse::GetNumTracks()
    {
        return sp_albumbrowse_num_tracks( m_pAlbumBrowse );
    }
    
    int AlbumBrowse::GetNumDiscs()
    {
        /// @todo parse the tracks for this album, to determine their disc indices, and thus the number of discs available
        return 0;
    }
    
    boost::shared_ptr<Disc> AlbumBrowse::GetDisc( int index )
    {
        /// @todo implement this function
        return boost::shared_ptr<Disc>();        
    }

	void SP_CALLCONV AlbumBrowse::callback_albumbrowse_complete( sp_albumbrowse* result, void* userdata )
	{
		AlbumBrowse* pAlbumBrowse = reinterpret_cast<AlbumBrowse*>( userdata );

		BOOST_ASSERT( pAlbumBrowse->m_pAlbumBrowse == result );

		pAlbumBrowse->OnComplete();
	}

}
