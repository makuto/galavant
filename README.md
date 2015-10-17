# Galavant
An open world action adventure game with emergent, fully simulated AI

Interested in working on a project with me? Galavant is a game intended to procedurally generate interesting agents and AI instead of fancy terrain. It's a big undertaking, but I feel it is the future in games. Many games have beautiful and complex worlds, but very, very few have interesting inhabitants.

With my current design, agents are based on Maslow's hierarchy of needs. This makes it quite easy to have diverse behaviours and emergent results - for example, if you build a wall around a town full of people and burn their crops, they will eventually start starving and might even resort to cannibalism!

Galavant is basically [Horizon](http://github.com/makuto/horizon) v2. I coded myself into a bit of a quagmire in Horizon, so I decided I would be more productive if I started over. 

Interested?
------------
Message me on Github or email me at macoymadson@gmail.com for any questions. I'd love some help!

There is a lot to the design of Galavant that does not fit in this readme. If you email me, I can fill you in on my goals with Galavant and what the game will actually be like. 

License
--------
The code is MIT licensed. I intend on keeping all data (images, sprites, gameplay/design data) private, following the Doom/Quake model. 

Technical
----------
I am writing Galavant in C++ (C++11, to be specific). I rely on SFML2 for sound, graphics, input, and networking. SFML is wrapped in my personal game library, [Base2.0](https://github.com/makuto/base2.0) (also MIT licensed), which also has other useful game-agnostic code. 

Horizon used a makefile for its build process (which I hand wrote). I have decided to abandon this method because I think it wasted a lot of my time updating it. I plan on using Codeblocks with GCC to make this process more automatic. It should also make compiling on Windows much simpler.
