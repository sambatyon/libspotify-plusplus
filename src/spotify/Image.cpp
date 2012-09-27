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

// local includes
#include "spotify/Image.hpp"
#include "spotify/Session.hpp"

namespace spotify {
Image::Image(boost::shared_ptr<Session> session) : image_(NULL), session_(session) {
}

Image::~Image() {
    Unload();
}

bool Image::Load(const byte *image_id) {
    image_ = sp_image_create(session_->session_, image_id);

    return (image_ != NULL);
}

void Image::Unload() {
    if (image_) {
        sp_image_release(image_);
        image_ = NULL;
    }
}

bool Image::IsLoading() {
    if (!image_)
        return false;
    if (!sp_image_is_loaded(image_))
        return true;
    return false;
}

const void *Image::GetData(size_t &outDataSize) {
    outDataSize = 0;

    if (!image_)
        return NULL;

    if (IsLoading()) {
        return NULL;
    }

    return sp_image_data(image_, &outDataSize);
}
}
