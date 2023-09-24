# About the project
A chess engine developed by Matthew Pechen-Berg, Ross Cleary, and Kristy Lau

## Running the Engine
1. download this repo somewhere
2. compile it by navigating to the repo's root directory, and running `make`
3. run it by navigating to the repo's root directory, and doing `./chess`

## Runtime Flags
`--auto`
- if this flag is provided to the executable, then whenever it is a computer's turn, they will immediately start their move, instead of waiting for your instruction. It is recommended to be used

## Game Commands
`game <whitePlayer> <blackPlayer>`
- this starts a match
- the players specified must be one of the following list:
  - `computer1`: a very weak AI which chooses random moves
  - `computer2`: a weak AI which prioritized capturing enemy pieces and putting the enemy king in check above all else
  - `computer3`: a weak AI which has the features of `computer2`, but also focuses on defending its own pieces (at a search depth of 1 still)
  - `computer4`: a strong AI which uses minimax and a complicated eval function to try making the best moves possible.
  - `human`: this allows a human to make the player's moves
 

`move`
- When it is a computer's turn, this command makes the computer think, then player their move

`move <startTile> <endTile>`
- the starting and ending tiles should be written in the following format: `A1`, `G8`, `F3`, etc.

`resign`
- this ends the game for the current player

## Note on the UI
The display for the engine uses UTF-8 characters for the chess pieces. If you are running the program on a display that modifies font colours from their standard (such as something with a Dark Mode), then it may appear that the colour of the pieces is swapped. So, keep in mind that white starts at the bottom, black starts at the top, and the board never flips (the first rank is always drawn at the bottom).