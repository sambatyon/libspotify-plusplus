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

namespace spotify {
Image::Image(boost::shared_ptr<Session> session) {
    session_ = session;
    pImage_ = NULL;
}

Image::~Image() {
    Unload();
}

bool Image::Load(const byte *image_id) {
    pImage_ = sp_image_create(session_->pSession_, image_id);

    return (pImage_ != NULL);
}

void Image::Unload() {
    if (pImage_) {
        sp_image_release(pImage_);
        pImage_ = NULL;
    }
}

bool Image::IsLoading() {
    if (NULL == pImage_) {
        return false;
    }

    if (!sp_image_is_loaded(pImage_)) {
        return true;
    }

    return false;
}

const void *Image::GetData(size_t &outDataSize) {
    outDataSize = 0;

    if (NULL == pImage_) {
        return NULL;
    }

    if (IsLoading()) {
        return NULL;
    }

    return sp_image_data(pImage_, &outDataSize);
}
}
