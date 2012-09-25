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
#include "spotify/Image.hpp"
#include "spotify/Session.hpp"

namespace Spotify {
Image::Image(boost::shared_ptr<Session> session) {
    m_session = session;
    m_pImage = NULL;
}

Image::~Image() {
    Unload();
}

bool Image::Load(const byte *image_id) {
    m_pImage = sp_image_create(m_session->m_pSession, image_id);

    return (m_pImage != NULL);
}

void Image::Unload() {
    if (m_pImage) {
        sp_image_release(m_pImage);
        m_pImage = NULL;
    }
}

bool Image::IsLoading() {
    if (NULL == m_pImage) {
        return false;
    }

    if (!sp_image_is_loaded(m_pImage)) {
        return true;
    }

    return false;
}

const void *Image::GetData(size_t &outDataSize) {
    outDataSize = 0;

    if (NULL == m_pImage) {
        return NULL;
    }

    if (IsLoading()) {
        return NULL;
    }

    return sp_image_data(m_pImage, &outDataSize);
}
}
