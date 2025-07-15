Mini-Pong
Mini-Pong is a simple 2-player cooperative Pong game developed using SDL.

Controls:

Player 1 moves their paddle using W and S keys.

Player 2 moves their paddle using the Up and Down arrow keys.

Gameplay:

The game starts when X is pressed.

The ball moves and players try to keep it in play together.

Press ESC to end the game at any time.

Compilation
Make sure you have SDL2 installed on your system. Then compile using:
gcc pong.c -o pong $(sdl2-config --cflags --libs) -lm
Running
Run the compiled executable:
./pong
