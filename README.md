## Pokemon Game

### Author: JJ SchraderBachar

### About

A pokemon inspired game written in C & C++ played in the terminal. It has Pokemon battles, trainer battles, and poke marts/centers. It uses Dijkstra's algorithm for trainer pathfinding.
It also uses then entire Pokedex for parsing of moves, pokemon, level-ups, types, etc.

### How to run

#### Make

Type `make` in the terminal

#### Clean

Type `make clean` in the terminal

#### Running

Run the command `./poke327` in the terminal to run the generated binary from the makefile.

This app also needs the Pokemon database provided in the pok327 folder. This should be placed in your computer's home directory for ease of loading the project. However, you may also put it in the project directory; you just need to update line 83 in db_parse.cpp

### Playing the game

Input a char, 7/y up & left, 8/k up, 9/u up & right, 6/l right, 3/n low right, 2/j down, 1/b lower left, 4/h left, 5 rest. > enter a pokebuiling if on it. t to display trainers. up/down arrow to scroll up/down on the trainer list. ESC to exit trainer list. Q to quit the game

When pressing esc, there is a bit of a delay so please be patient and don't hit it twice.

Press `f` to fly to a map, (-200,-200) to (200,200). 

Press `p` to teleport to a random place on the map as well.
