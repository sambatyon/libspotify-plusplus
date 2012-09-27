/*
 * Copyright 2012 Alexander Rojas
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

#if defined(WIN32)
#   if defined(libspotifypp_EXPORTS)
#       define LIBSPOTIFYPP_API _declspec(dllexport)
#   else
#       define LIBSPOTIFYPP_API _declspec(dllimport)
#   endif
    // This pragma disables the error of exporting templates (which is a hassle in windows). Use it only if your client
    // app uses the same compiler you use to compile this library
#   pragma warning(disable:4251)
#else
#   define LIBSPOTIFYPP_API
#endif
