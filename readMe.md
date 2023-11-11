# Robert Robot

## Code Structure

The code is split into:  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Data Structures  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Drawing functions  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Robot functions  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Algorithm functions  

The variables at the top of the code are declared globally by choice in order to make the code neater. In addition, each variable does not need to be copied into a parameter multiple times over, which makes the code fractionally more efficient as well.  

The large bulk of programming logic to draw the animations is done in the drawing functions. The main function will draw the initial grid, walls, and home, by calling the drawBackground(). The marker and the robot are the in the foreground which means they get redrawn each time the robot moves.  

The robot functions are the default functions given in the coursework.  

Algorithm functions are all the methods used in order to move the robot and solve the puzzle. They regularly call the robot functions, but they do not have access to the values in the robot functions. So the algorithm functions immitate a real life robot, solving the puzzle only from what it can see infront, and from what it can remember it already did. 

## Algorithm Logic
The logic of the complexSolve() algorithm is as follows:
1. Check if we are on a marker, if so pick up and go home
2. If not, turn left 
3. Check if you can move forward and that you have not been to this square
4. If yes, move forward, store this position's relative coordinates in the visitedList, and call yourself again (resulting in another left turn)
5. If no, turn right and try the same
6. If you still cannot move forward after 2 right turns, go back to the block you were last on
7. Since the function had called itself before the previous function turned right twice, returning the function 
    will cause the previous instance to turn right, and try this path instead.
8. If it can move forward after this right turn, the robot goes to another path,
    if it fails to move forward after 2 turns again, it moves back another block

Essentially I treat every block on the grid as a junction in a maze, and search through every possible option in a grid,
while avoiding blocks I have already visited, or blocks like walls and home. 

This algorithm therefore only uses the 1 sensor that the robot has access to (check forward), and information it can store, like which moves it has already made. Using these moves it can store its direction and the positions it visited relative to where it was initially placed. Hence this robot stays true to real life and does not use any information from within the code to solve the puzzle, which was my goal.  

These are the criteria I placed on the program when developing it to keep true to a real life robot and solve this puzzle:
1. The robot cannot know its exact place on the grid since the initial placement is essentially random  (it can only calculate its relative position to where it started, no using exact 2D array replica of the grid)
2. The robot cannot differentiate whether it has hit a block, or it has reached the edge of the grid (only whether it can or cannot move forward, that is the sole function of a forwards facing sensor)
3. The robot animates smoothly (no snapping)
4. The grid is fully resizeable, and fully customiseable with walls and markers
5. The robot solves any combination of walls and markers placements as long as it is possible

This version of the program succeeds in all of these criteria. 

## How to run the code

The code takes in 6 parameters:  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; -waitTime (ms wait for the animation speed, lower means faster, suggested 20 for slow 5 for speedy)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; -gridSize (it will create a SIZExSIZE grid)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; -screenResolutionY (the drawing is scaled in order to fit the resolution of any screen, I used 982 pixels for my machine, insert the height in pixels of the host machine)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; -wallLocations: input "-." before the coordinates. Then input the X and Y coordinates of the home, separated by a period. And then followed by the X and Y locations of however many walls, also separated by periods. Example might look like "-.1.0.1.1.1.3" Where (1,0) is the home and the rest of the coordinates like (1,1) represent walls.  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; -markerLocations: the same format as wallLocations, input the marker locations separated by periods. Example might look like "-.5.5.0.6" for two markers at (5,5) and (0,6).

Compile the code using:
```
gcc -o main main.c graphics.c
```


And run the code using:
```
./main waitTime gridSize screenResolutionY wallLocations markerLocations | java -jar drawapp.jar
```

Note: I changed my drawapp to be called just "drawapp.jar" for simplicity

With the parameters in place an example run code would be:
```
./main 20 8 982 -.1.0.1.1.1.3.3.0.3.1.3.2.3.3 -.5.5.0.6 | java -jar drawapp.jar
```

There is no input validation in this program as it was not in the scope of the project. Please make sure the input parameters are given in the requested format to ensure the code working. 

## Thank you