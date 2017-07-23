# Galavant
An open world action adventure game with emergent, fully simulated AI

Interested in working on a project with me? Galavant is a game intended to procedurally generate interesting agents and AI instead of fancy terrain. It's a big undertaking, but I feel it is the future in games. Many games have beautiful and complex worlds, but very, very few have interesting inhabitants.

With my current design, agents are based on Maslow's hierarchy of needs. This makes it quite easy to have diverse behaviours and emergent results - for example, if you build a wall around a town full of people and burn their crops, they will eventually start starving and might even resort to cannibalism!

Galavant is basically [Horizon](http://github.com/makuto/horizon) v2. I coded myself into a bit of a quagmire in Horizon, so I decided I would be more productive if I started over. 

## Building

Galavant uses Flatbuffers. Before trying to build, make sure to initialize it:
`git submodule update --init --recursive`
Then run cmake in `thirdParty/flatbuffers`:
`cmake .`
Then
`make`

Jam is used as the build system for Galavant. I used to use makefiles in the Horizon iteration of this project, and I hated them. Jam is much easier to use and takes much less work to get a project set up. If people actually become interested in this project, I can invest some time in cmake-ifying it for more standard building. If you're willing to try building using Jam, you may have to edit Jamrules to set your preferred compiler (the default is g++).

To build with jam, first install jam:
`sudo apt-get install jam`
Then run jam (in galavant/ is fine):
`jam`
For vastly faster build times, add `-j` plus the number of CPU cores on your machine, e.g.:
`jam -j4`

## Dependencies

Galavant uses features of C++11.

The following libraries are required by Galavant and included in /thirdParty:
- [OpenSimplexNoise](https://gist.github.com/tombsar/716134ec71d1b8c1b530), created by Arthur Tombs (public domain)
- [Flatbuffers](https://github.com/google/flatbuffers), created by Google/Fun Propulsion Labs (Apache License, v2.0)
- [Catch](https://github.com/philsquared/Catch), created by various contributors/a dude named Travis (Boost Software License)

## License

The code is MIT licensed. I intend on keeping all data (images, sprites, gameplay/design data) private, following the Doom/Quake model, but for now, consider those to be MIT Licensed.

## Want to Get Involved?

Message me on Github or email me at macoymadson@gmail.com for any questions. I'd love some help!

There is a lot to the design of Galavant that does not fit in this readme. If you email me, I can fill you in on my goals with Galavant and what the game will actually be like. 