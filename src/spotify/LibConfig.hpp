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