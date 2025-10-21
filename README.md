# Evil Ass-Wizard Game

You are the **Evil Ass-Wizard** (your name's origin: you're an evil ass [as in donkey], and you're a wizard). Your task is to climb the **Infinite Spire** of your arch nemesis, the **Evil-Ass Wizard** (he's not an ass, just evil), and get the... whatever is on the top of it.
But beware! For there are vile minions, cadavers and other otherworldy beings guiding the tower, and you must defeat them all to progress, lest you perish...
Your task isn't as simple as just shouting "FIREBALL!", though: You have a mana counter (not actually implemented yet lol) that you can use to do all sorts of things: cast fireballs, heal yourself, etc.
However, this mana counter does not replenish itself automatically: You have to kill enemies and/or pick up elixir potions to regain it! Thus, you must be quite smart and tactical: Even an accidental fireball can cast doom upon you!

But out of the story: **Evil Ass-Wizard Game** is a 2.5D raycaster-based shooter game where you have to progress to (atleast I plan it this way) procedurally generated mazes.

The game's progress right now:
- Raycasting
- Textured walls
- Textured floors

With these coming:
- Sprites
- Mana counter
- Procedural generation

Yeah I know I know, I don't have that much yet, but to my defense 10 hours is surprisingly short '^^
I think this game has a lot of potential; I really like raycasters and 2.5D games, and I even made a raycaster engine back in 2024 (https://github.com/LBence-Szixi/lakys_awsome_raycaster/).
That one used GLFW though, and this time I'm using Raylib, which is a pretty cool library I've started learning recently. It's really fun and surprisingly much faster than my old implementation;
And as a sweet bonus it has a Nintendo Switch port so I can even port this game to be Switch Homebrew if I want :P

# How to play
1. Download newest release
2. Unzip
3. launch `EvilAssWizardGame.exe`

**Linux build coming soon!**

# Controls

- WASD to look around and move
- E to interact with objects

# How to build
1. Download and unzip the latest version.
2. Set up a development environment with Raylib (Any development environment should be fine as long as it has a Raylib port)
3. Build the game with the Makefile
4. Enjoy!

**WARNING: So far I have only tested the code on Windows. Although there should not be anything that would bar this from building on Linux or any other platform, I can not guarantee that it will build on said platforms.**
