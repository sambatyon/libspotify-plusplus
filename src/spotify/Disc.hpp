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
#include <vector>

// boost includes
#include <boost/shared_ptr.hpp>

namespace spotify {
// forward declaration
class Track;
class Album;

/// @class Disc
/// @brief A collection of tracks making up a disc of an album
/// @brief (where an album could have multiple discs)
class Disc {
public:
    int GetNumTracks();

    boost::shared_ptr<Track> GetTrack(int index);

    boost::shared_ptr<Album> GetAlbum();

    int GetDiscIndex();

private:

    /// @todo be careful that Disc and Album don't have shared_ptr's pointing at each other..  weak_ref?
    boost::shared_ptr<Album> album_;

    std::vector< boost::shared_ptr<Track> > tracks_;
};
}