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

#pragma once

// std includes
#include <string>

// libspotify include
#include "libspotify/api.h"

// boost includes
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace spotify {
// forward declarations
class Session;

class PlayListElement : virtual public boost::enable_shared_from_this<PlayListElement> {
public:
    PlayListElement(boost::shared_ptr<Session> session);
    virtual ~PlayListElement();

    virtual boost::shared_ptr<PlayListElement> GetParent() const;
    virtual void SetParent(boost::shared_ptr<PlayListElement> parent);

    virtual bool HasChildren() = 0;
    virtual int GetNumChildren() = 0;
    virtual boost::shared_ptr<PlayListElement> GetChild(int index) = 0;

    virtual bool IsLoading(bool recursive) = 0;

    virtual std::string GetName() = 0;

    enum eType {
        PLAYLIST = 0,
        PLAYLIST_FOLDER,
        PLAYLIST_CONTAINER,
        TRACK
    };

    virtual eType GetType() = 0;

    virtual void AddPlayList(boost::shared_ptr<PlayListElement> playList) {}

    virtual void DumpToTTY(int level = 0) = 0;

    void				*GetUserData();
    void				SetUserData(void *pUserData);

    boost::shared_ptr<Session> GetSession();

protected:

    boost::weak_ptr<PlayListElement>	parent_;

    boost::shared_ptr<Session>	session_;

private:
    void				*pUserData_;
};

}