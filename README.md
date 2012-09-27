ABOUT
=====

A C++ Wrapper for [libspotify](https://developer.spotify.com/technologies/libspotify/).

This was originally a fork from JimKnowler/libspotify-plusplus; however in the last few
days I have refactored so much that I don't hope to merge it back again.

Dependencies
- libspotify 12 (as provided by spotify)
- boost c++ libraries
- log4cplus
- A C++ compiler which has the same level of support for C++11 as VC++ 2012 does
- CMake 2.8
- python at least 2.6 in order to run the style checker.

Helping
-------
I'm just starting, but it is obvious that I will be using boost.test in order to do unit
tests, before requesting a pull, be sure that all tests pass. If you need to modify a test
for some reason, please contact me and we will discuss it.

I try to follow google styleguide with some exceptions. Lines are allowed to be 120 characters
long, indentation is 4 characters, indentation for the keywords `public`, `proteted` and `private`
are two spaces. Don't indent namespaces, please! C++ is ok as it is boost.

When boost and C++11 stl offer the same functionality, choose the stl (I know, I still use
the boost shared pointers, I haven't change them yet).


TODO (unordered collection of ideas for forthcoming work)
---------------------------------------------------------

- Change the GetType functions of the `PlayListElement` for visitor functions
- Favor Observers over inheritance as was done in the original.
- Moreover, use signals/slots ala Qt, using boost signals instead of observers.

### These are the todo points of JimKnowler I still don't know if keep them or remove them altogether

Spotify/Disc
 - be able to ask an album for the number of discs that it contains
 - be able to obtain a 'disc' object for each of them (with references to the tracks in parent album)

?? Cache raw pointers in look-up table, or associate userdata with them?
 - so that we can always return the same C++ handle for the same artist/track/album etc
   - i.e. when retrieving Album from a track, and from an ArtistBrowse

Different behaviour on spottily objects while they are loading
 - throw exception?

Refactor IsLoading() as HasLoaded() to match libspotify
 - perhaps refactor all functions to more closely match libspotify api

"Loaded Callbacks" where supported