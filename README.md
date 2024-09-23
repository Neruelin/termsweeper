# Termsweeper
Minesweeper in the terminal. The controls are simple, input an pair of x y integer coordinates in the format `x y` to check a cell. If all safe cells are checked the game with end with a win. If an unsafe cell is checked the game will end with a loss and the map will be revealed. Safe cells that are adjacent to one or more mines will display an integer 1-8 when checked.

## Build
### Requirements
- gcc
- make 
- cmake
- libxi-dev `sudo apt install libxi-dev`
- libx11-dev `sudo apt install libx11-dev`
- libgl1-mesa-dev `sudo apt install libgl1-mesa-dev`
- freeglut source `wget -qO- https://github.com/freeglut/freeglut/archive/refs/tags/v3.6.0.tar.gz | tar xvz`

Using `make`, run `make build && ./termsweeper` or `make run`;