# Maze
This application allows you to play with mazes and look at the solution.

## Game
There are two game you can play:

1. Mouse and cheese (MOUSE_GAME)
	You are a mouse and you have to go from the top-left corner of the maze to a random cell in the maze where you will find the cheese. The random cell is not exactly random as it is the most **distant cell**, so it will never happen that the cell will be just next to you.

2. Escape Game (MOUSE_GAME)
	You have to escape from the entry to the exit. The exit will be on the wall. Its position is random generated (again, it is the furthest wall from you).

## Install
To install the app, I created a make file, do as follows:

	make

## Play
To play the app you can just do:

	./maze

This will generate a random .maze and a .solution files titled with their timestamps and with a random colourscheme.

If you want to play later the game, you can alway play with

	# Play
	cat MAZEYOUWANT.maze | java -jar drawapp.jar

	# Watch solution
	cat MAZEYOUWANT.solution | java -jar drawapp.jar

### Levels
Levels are just pre-configured settings. If you use `-x`, `-y` configs, the pre-configured setting will be replaced.

### Breadcrumbs
Breadcrumbs will help you to go through the maze. Default breadcrumbs is 0, so there will not be any.
If you set breadcrumbs to be 5 `-b5`, every 5 steps you are required to do, there will be a breadcrumb.
Follow them to find the solution.

### Colourschemes
We have 4 pre-sets:

1. Default `-c1`
1. Star wars `-c2`
1. Funky `-c3`
1. Black/White `-c4`

If you set -c0 it will be random chosen.

### Play later with `-s`
When you generate a maze it starts directly on your drawapp. If you don't want to play directly, use `-s`


### Optional command line inputs
To get all the optional commands do:

	./maze -h
	/maze [-x<int>] [-y<int>] [-l<int>] [-g<int>] 

	Usage:
  	-x<int>			Horizontal cells (default: 10)
	  -y<int>		Vertical cells (default: 10)
	  -l<int>		Level from 1 to 5 (default: 2)
	  -b<int>		Frequency of breadcrumbs, 0 is no breadcrumbs (default: 0)
	  -g<int> 		Game: 1 for Mouse&cheese, 2 for Escape (default: 1)
	  -c<int>		Colourscheme: 1 for Default, 2 for Star Wars, 3 for Funky, 4 for B/W
	  -s			Silently create maze files without starting draw
	  -r          	Delete all the previous *.maze and *.solution
